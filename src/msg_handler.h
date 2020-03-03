#ifndef MSG_HANDLER_H
#define MSG_HANDLER_H

#include <string>
#include <sstream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <thread>

// mbed headers
#include "mbed.h"

// bitcoin/dev board headers
#include "Bitcoin.h"
#include "deletedlib.h"
#include "fs_handler.h"
#include "hardware.h"

// nanopb Headers
#include "messages-bitcoin.pb.h"
#include "messages-common.pb.h"
#include "messages.pb.h"
#include "messages-management.pb.h"
#include "pb_common.h"
#include "pb_decode.h"
#include "pb_encode.h"

/*
taken from https://github.com/trezor/trezor-common/blob/master/protob/protocol.md

First packet
--------|---------------|-------------------|-------------------------------------------|
offset  |    length     |    type           |        contents                           |
--------|---------------|-------------------|-------------------------------------------|
    0   |        3      |    char[3]        |        '?##' magic constant               |
--------|---------------|-------------------|-------------------------------------------|
    3   |        2      |    BE uint16_t    |        numerical message type             |
--------|---------------|-------------------|-------------------------------------------|
    5   |        4      |    BE uint32_t    |        message size                       |
--------|---------------|-------------------|-------------------------------------------|
    9   |        55     |    uint8_t[55]    |        first 55 bytes of message encoded  |
        |               |                   |        (padded w/ zeroes if need be)      |
--------|---------------|-------------------|-------------------------------------------|


Following packets
--------|---------------|-------------------|-------------------------------------------|
offset  |    length     |    type           |        contents                           |
--------|---------------|-------------------|-------------------------------------------|
    0   |        1      |    char[1]        |        '?' magic constant                 |
--------|---------------|-------------------|-------------------------------------------|
    1   |        63     |    uint8_t[63]    |        63 bytes of message encoded        |
        |               |                   |        (padded w/ zeroes if need be)      |
--------|---------------|-------------------|-------------------------------------------|
*/

// __attribute__ packed
struct __attribute__ ((packed)) trezor_first_message { //structure of first protobuf packet above
    char header[3] = {'?', '#', '#'};
    uint16_t type = 0;
    uint32_t size = 0;
    uint8_t message[55] = {0};
};

struct __attribute__ ((packed)) trezor_subsequent_message { //structure of subsequent/following protobuf packet above
    char header = '?';
    uint8_t message[63] = {0};
};

class TrezorMessageHandler {
        FSHandler& fs_handler;
        HID_REPORT recv_report = {.length = 64, .data = {0}};
        HID_REPORT send_report = {.length = 64, .data = {0}};
        uint16_t msgtype; //type of message (look in messages.pb.h for list of types)
        uint32_t msglen = 0; // size of message
        uint8_t *message; //message w/o all protobuf headers
        uint32_t remainder; //remainder of message (if any) to be received in subsequent packets

    public:

        TrezorMessageHandler(FSHandler&);

        void serialRxInterruptHandler();

        void processEvents();

        void init(); //responsible for setting up FS handler

        //serial communication functions
        int send_data(void* buffer);

        int recv_data(uint8_t* buffer);

        int unpack_data(uint8_t buffer[64]); //decode protobuf message after receiving

        int pack_data(hw_trezor_messages_MessageType type, const pb_msgdesc_t *fields, const void* res); //prepare protobuf message for sending

        //protobuf message handlers
        void initialize_handler();

        void prepare_vault_handler();

        void finalize_vault_handler();

        void get_features_handler();

        void unvault_handler();

        void check_vault_bal_handler();

        void check_unvault_bal_handler();
};

#endif
