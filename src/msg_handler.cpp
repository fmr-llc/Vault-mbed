#include "msg_handler.h"

TrezorMessageHandler::TrezorMessageHandler(FSHandler& fs)
    : fs_handler(fs)
{
    serial.attach(callback(this, &TrezorMessageHandler::serialRxInterruptHandler), Serial::RxIrq);
}

void TrezorMessageHandler::serialRxInterruptHandler() {
    led1 = !led1;
    serial.putc(serial.getc());
    // FIXME put into 64b buffer, set some flag so processEvents knows when it
    // has a full message to process
}

void TrezorMessageHandler::processEvents() {
    if(hid.read_nb(&recv_report)) {
        led2 = !led2;
        unpack_data(recv_report.data);
    }
}

int TrezorMessageHandler::send_data(void* buffer){
    memcpy(send_report.data, buffer, 64);
    hid.send_nb(&send_report);
    led4 = !led4;
    printf("Sent response:\n");
    printf("hex: ");
    for(int i=0;i<64;i++)
        printf("%02x", send_report.data[i]);
    printf("\nstr: ");
    for(int i=0;i<64;i++)
        printf("%c", send_report.data[i]);
    printf("\n");
    return 0;
}

//unpack data from computer in 64 byte protobuf chunks
int TrezorMessageHandler::unpack_data(uint8_t buffer[64]) {
    trezor_first_message* m = (trezor_first_message*)buffer;
    trezor_subsequent_message* n = (trezor_subsequent_message*)buffer;

    //check if message received is first packet
    //fill necessary fields
    if(string((char*)m->header) == "?##") {

        msgtype = m->type = __builtin_bswap16(m->type);
        msglen = m->size = __builtin_bswap32(m->size);
        message = (uint8_t *) malloc(msglen);
        memcpy(message, (buffer+9), min((uint32_t)55, msglen));
        remainder = msglen - min((uint32_t)55, msglen);

    //check if message received is subsequent packet
    //fill necessary fields
    } else if(n->header == '?') {

        //move the pointer to the current index to populate with remainder of message
        //if not, the original 55 bytes would be rewritten
        memcpy(message+(msglen - remainder), (buffer+1), min((uint32_t)63, remainder));
        remainder -= min((uint32_t)63, remainder);

    } else {
        printf("ERROR: USBHID message received with invalid header.\n");
    }

    if(remainder == 0) { // full message received

        // list of trezor message types found below
        // https://github.com/bitcoin-core/HWI/blob/master/hwilib/devices/trezorlib/messages/MessageType.py
        //call necessary message handler depending on message type received
        switch(msgtype) {

            case hw_trezor_messages_MessageType_MessageType_DevBoardInitialize:
                initialize_handler();
                break;

            case hw_trezor_messages_MessageType_MessageType_PrepareVault:
                prepare_vault_handler();
                break;

            case hw_trezor_messages_MessageType_MessageType_FinalizeVault:
                finalize_vault_handler();
                break;

            case hw_trezor_messages_MessageType_MessageType_UnvaultRequest:
                unvault_handler();
                break;

            case hw_trezor_messages_MessageType_MessageType_CheckVaultBalance:
                check_vault_bal_handler();
                break;

            case hw_trezor_messages_MessageType_MessageType_CheckUnvaultBalance:
                check_unvault_bal_handler();
                break;

            //called on initialization. respond with type 17 (features)
            case hw_trezor_messages_MessageType_MessageType_GetFeatures:
                printf("GetFeatures message received.\n");
                get_features_handler();
                break;

            default :
                break;
                //message type error, message type not recognized
        }


        return 0; //full message received, tell the board to stop bugging cmp
    }

    return 1;
}

void TrezorMessageHandler::initialize_handler(){
    //initialize active/clawback xpubs into deletedlib

}

void TrezorMessageHandler::get_features_handler(){

    hw_trezor_messages_management_GetFeatures req      = hw_trezor_messages_management_GetFeatures_init_default;
    hw_trezor_messages_management_Features res         = hw_trezor_messages_management_Features_init_default;

    pb_istream_t stream_i = pb_istream_from_buffer(message, sizeof(message));
    pb_decode(&stream_i, hw_trezor_messages_management_GetFeatures_fields, &req);

    strncpy(res.vendor, "STMicroElectronics", 19);
    res.has_vendor = true;

    pack_data(hw_trezor_messages_MessageType_MessageType_Features,
              hw_trezor_messages_management_Features_fields,
              &res);
}

void TrezorMessageHandler::prepare_vault_handler(){
    hw_trezor_messages_bitcoin_PrepareVault req         = hw_trezor_messages_bitcoin_PrepareVault_init_default;
    hw_trezor_messages_bitcoin_PrepareVaultResponse res = hw_trezor_messages_bitcoin_PrepareVaultResponse_init_default;

    pb_istream_t stream_i = pb_istream_from_buffer(message, sizeof(message));
    pb_decode(&stream_i, hw_trezor_messages_bitcoin_PrepareVault_fields, &req);

    generateKey();
    string address = getAddress();
    Script script = getScript();
    Signature sig = signMessage((uint8_t*)req.signThis);

    strncpy(res.address, address.c_str(), address.length());
    strncpy(res.redeemScript, script.toString().c_str(), script.toString().length());
    strncpy(res.sig, sig.toString().c_str(), sig.toString().length());

    pack_data(hw_trezor_messages_MessageType_MessageType_PrepareVaultResponse,
              hw_trezor_messages_bitcoin_PrepareVaultResponse_fields,
              &res);
}

void TrezorMessageHandler::finalize_vault_handler(){

    hw_trezor_messages_bitcoin_FinalizeVault req         = hw_trezor_messages_bitcoin_FinalizeVault_init_default;
    hw_trezor_messages_bitcoin_FinalizeVaultResponse res = hw_trezor_messages_bitcoin_FinalizeVaultResponse_init_default;

    pb_istream_t stream_i = pb_istream_from_buffer(message, sizeof(message));
    pb_decode(&stream_i, hw_trezor_messages_bitcoin_FinalizeVault_fields, &req);

    string s = constructTx(req.hex); //returning the txid

    strncpy(res.txid, s.c_str(), s.length());
    res.isDeleted = true;

    pack_data(hw_trezor_messages_MessageType_MessageType_FinalizeVaultResponse,
              hw_trezor_messages_bitcoin_FinalizeVaultResponse_fields,
              &res);
}

void TrezorMessageHandler::unvault_handler(){
    hw_trezor_messages_bitcoin_UnvaultRequest req         = hw_trezor_messages_bitcoin_UnvaultRequest_init_default;
    hw_trezor_messages_bitcoin_UnvaultResponse res        = hw_trezor_messages_bitcoin_UnvaultResponse_init_default;

    pb_istream_t stream_i = pb_istream_from_buffer(message, sizeof(message));
    pb_decode(&stream_i, hw_trezor_messages_bitcoin_UnvaultRequest_fields, &req);

    FILE *f = fs_handler.open((char*)req.txid, 0);
    int size = fs_handler.get_size(f);

    fgets(res.hex, size, f);
    fs_handler.close(f);

    pack_data(hw_trezor_messages_MessageType_MessageType_UnvaultResponse,
              hw_trezor_messages_bitcoin_UnvaultResponse_fields,
              &res);
}

void TrezorMessageHandler::check_vault_bal_handler(){

}

void TrezorMessageHandler::check_unvault_bal_handler(){

}

int TrezorMessageHandler::pack_data(hw_trezor_messages_MessageType type, const pb_msgdesc_t *fields, const void* res){
    int msgsize;
    uint8_t* response;
    trezor_first_message m;
    trezor_subsequent_message t;

    // Create a pb_ostream_t with the encoded data in the buffer `response`
    pb_get_encoded_size((size_t*)&msgsize, fields, res);
    response = new uint8_t[msgsize];
    pb_ostream_t stream_o = pb_ostream_from_buffer(response, msgsize);
    pb_encode(&stream_o, fields, res);

    m.type = __builtin_bswap16(type);
    m.size = __builtin_bswap32(msgsize);
    memcpy(m.message, response, min(55, msgsize));
    int msgremaining = msgsize - min(55, msgsize);

    send_data(&m);

    // Pack rest of the message into subsequent packets
    while(msgremaining > 0) {
        //copying either 63 bytes, or the remainder of message into t.message
        memcpy(t.message, response+(msgsize-msgremaining), min(63, msgremaining));
        msgremaining -= min(63, msgremaining);
        send_data(&t);
    }

    free(response);

    return 0;
}
