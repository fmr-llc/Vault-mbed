#ifndef FS_HANDLER_H
#define FS_HANDLER_H

// mbed os headers
#include "mbed.h"
#include "BlockDevice.h"
#include "LittleFileSystem.h"

class FSHandler {
public:

    FSHandler();
    FILE *open(char* txid, int mode); //0 for read, 1 for write
    void close(FILE *f);
    int get_size(FILE *f);
};

#endif
