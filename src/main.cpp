#include "hardware.h"
#include "USBPhyHw.h"
#include "usb_phy_api.h"

// bitcoin lib
#include "Bitcoin.h"
#include "helpers.h"
#include "deletedlib.h"

// handler librariers
#include "msg_handler.h"
#include "fs_handler.h"
#include "interface.h"

// define the following to disable the GUI
#include <tft.h>
#include <touchpad.h>
#define DISABLE_GUI     // Define this to disable the GUI and enable USBHID
//#undef DISABLE_GUI    // Or undefine it to enable the GUI and disable USBHID

/******** Hardware interface globals ***********/
GUI gui;                                    // LVGL gui
RawSerial serial(USBTX, USBRX);             // Serial (debug)
RNG_HandleTypeDef rng;                      // Hardware RNG
QSPI_DISCO_F469NI qspi;                     // Internal mnemonic storage
USBHID hid(true, 64, 64, 0x534c, 0x0001);   // trezor USB HID
DigitalOut led1(LED1);                      // LED 1 (leftmost)
DigitalOut led2(LED2);                      // LED 2
DigitalOut led3(LED3);                      // LED 3
DigitalOut led4(LED4);                      // LED 4 (rightmost)

/** Temporary globals (FIXME needs refactor) ***/
FSHandler*           fs_handler_ptr;

/******************* Main part *****************/
int main(){
    printf("main()\n");
    FSHandler            fs_handler;
    TrezorMessageHandler msg_handler = TrezorMessageHandler(fs_handler); //message handler for protobuf

    if(!hid.configured()) {
        printf("HID is unconfigured?!?!\n");
    }
    if(!hid.ready()) {
        printf("HID is not ready?!?!\n");
    }

#ifndef DISABLE_GUI
    gui.init();
    showInitScreen();
    printf("gui init done\n");
#else
    printf("gui is disabled\n");
#endif
    initRNG();
    qspi.Init();

    while(1){
#ifndef DISABLE_GUI
        gui.update();
#endif
        msg_handler.processEvents();
        wait_us(100000);
        led3 = !led3;
    }
}

