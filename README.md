# Vault-mbed

## Installing

This is a prototoype bitcoin hardware wallet that implements the Trezor
communications protocol using `nanopb`.  The intended device is the STM32F469
dev board [1], though many similar mbed-os devices would work.

To build the project, run:

    sudo apt-get install protobuf-compiler libglib2.0-bin util-linux
    pip3 install -r requirements.txt
    make

To install the firmware to the device run:

    make install

It will attempt to guess the usbstorage device on linux by looking at dmesg, or
lsblk, mount it and install. You must connect the mini-usb connector on the top
of the board for this, and the jumper JP2 on the back must be set to STLK. We
use `gio` for this (part of libglib -- `libglib2.0-bin` ubuntu/debian package)
and lsblk (part of `util-linux` ubuntu/debian package).

An alternative method to mount the device and install the firmware is provided
that uses `sudo` if you have root access on your computer. To use it run:

    make sudoinstall

Edit the `Makefile` to change the mount point or `sudo` command. This is for if
your user cannot access the usbstorage device in /dev. You can also change the
permissions on the device in /dev so you can access it.

Application communication with the device is done using HWI [2] for which the
device presents itself as a Trezor device. For this the micro-usb connector must
be connected to your computer (NOT the same as the mini-usb connector used to
install the firmware in the previous paragraph.

## Running

Install the HWI[2] library.

    ./hwi.py enumerate

## Debugging

The mbed device provides a serial port for debugging messages. To see these
messages you can run:

    mbed sterm --port /dev/ttyACM0

For this your user must have access to the serial device presented
`/dev/ttyACM0`. If the above command does not work you can:

    chmod 0666 /dev/ttyACM0

But you'll have to do this every time you plug in the device. Or install the
`udev` rules file provided in the `udev/` subdirectory:

    sudo cp udev/51-stm32f4-discovery.rules /etc/udev/rules.d/
    sudo udevadm trigger

There's a line (commented out) in this file that will additionally make the
usbstorage device read/writable by users (for installing compiled firmware), but
you shouldn't need it if you have `gio` installed as described above.

Once all that is working, you can use `serial.printf()` to send debugging
messages to this serial console.

.. [1] https://www.st.com/en/evaluation-tools/32f469idiscovery.html

.. [2] https://github.com/bitcoin-core/HWI
