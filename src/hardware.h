#ifndef __HARDWARE_H__
#define __HARDWARE_H__

#include "mbed.h"
#include "QSPI_DISCO_F469NI.h" // internal storage
#include "USBHID.h"
#include "QSPI_DISCO_F469NI.h"
#include "gui.h"

/* Definitions for global variables representing hardware on the board */
extern RawSerial serial;
extern USBHID hid;
extern DigitalOut led1;
extern DigitalOut led2;
extern DigitalOut led3;
extern DigitalOut led4;
extern QSPI_DISCO_F469NI qspi;
extern RNG_HandleTypeDef rng;
extern GUI gui;


#endif
