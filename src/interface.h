#ifndef INTERFACE_H
#define INTERFACE_H

#include "hardware.h"

// bitcoin lib
#include "Bitcoin.h"
#include "helpers.h"
#include "deletedlib.h"

// handler librariers
#include "msg_handler.h"
#include "fs_handler.h"

void showInitScreen();
lv_res_t constructTxCallback(lv_obj_t * btn);
lv_res_t toInitMenuCallback(lv_obj_t * btn);
lv_res_t showSerialCallback(lv_obj_t * btn);
lv_res_t serialWriteCallback(lv_obj_t * btn);
lv_res_t serialReadCallback(lv_obj_t * btn);

#endif
