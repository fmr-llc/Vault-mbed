#ifndef __HELPER_H__
#define __HELPER_H__

#include "mbed.h"
#include <string>
#include <sstream>
#include <stdint.h>
// to use string class without std::
using namespace std;

/*********** random number generator ***********/

// initializes TRNG
int initRNG();

// generates a single random number
uint32_t getRandomNumber(void);

// fills the buffer with random data
size_t getRandomBuffer(uint8_t * arr, size_t len);

/*************** mnemonic stuff ***************/

// loads mnemonic from the internal storage
string loadMnemonic();

// saves mnemonic to the internal storage
int saveMnemonic(string mnemonic);

// wipes the device (deletes mnemonic from the memory)
int wipe();

#endif
