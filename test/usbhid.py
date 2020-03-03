#!/usr/bin/env python3
#
#Simple example on how to send and receive data to the Mbed over USB (on Linux) using pyUSB 1.0
#
import os
import sys

import usb.core
import usb.util

from time import sleep
import random

import hid

# handler called when a report is received
def rx_handler(data):
    print('recv: ', data)

def findHIDDevice(mbed_vendor_id, mbed_product_id):
    # Find device
    hid_device = usb.core.find(idVendor=mbed_vendor_id,idProduct=mbed_product_id)

    if not hid_device:
        print("No device connected")
    else:
        sys.stdout.write('mbed found\n')
        if hid_device.is_kernel_driver_active(0):
            try:
                hid_device.detach_kernel_driver(0)
            except usb.core.USBError as e:
                sys.exit("Could not detach kernel driver: %s" % str(e))
        try:
            hid_device.set_configuration()
            hid_device.reset()
        except usb.core.USBError as e:
            sys.exit("Could not set configuration: %s" % str(e))

        endpoint = hid_device[0][(0,0)][0]

        while True:
            data = [0x0] * 16

            #read the data
            bytes = hid_device.read(endpoint.bEndpointAddress, 8)
            rx_handler(bytes);

            for i in range(8):
                data[i] = bytes[i]
                data[i+8] = random.randint(0, 255)

            hid_device.write(1, data)

if __name__ == '__main__':
    # The vendor ID and product ID used in the Mbed program
    mbed_vendor_id = 0x534c
    mbed_product_id = 0x0001
    dev = None

    for d in hid.enumerate():
        if d['vendor_id'] == mbed_vendor_id and d['product_id'] == mbed_product_id:
            dev = hid.device()
            print("opening: ", d['path'])
            try:
                dev.open_path(d['path'])
            except Exception as e:
                print(e)
                raise e
            break

    if dev is None:
        print("Unable to find USB device with vendor ID 0x%04x, product ID 0x%04x"%(mbed_vendor_id, mbed_product_id))
    else:
        print("HID found device")
    dev.set_nonblocking(False)

    while True:
        # Sends the GetFeatures message
        r = dev.read(8)
        if(r):
            print("Received: ", r)
        dev.write(b'?##\x007\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00')

    # Search the Mbed, attach rx handler and send data
    #findHIDDevice(mbed_vendor_id, mbed_product_id)
