#include "helpers.h"
#include "hardware.h"

// internal storage we will use to store mnemonic
#define WRITE_READ_ADDR     ((uint32_t)0x0050)
#define QSPI_BASE_ADDR      ((uint32_t)0x90000000)

// magic constant to check if we saved the mnemonic already
static const char magic[] = "mnemonic";

string loadMnemonic(){
    char buffer[300] = "";
    qspi.Read((uint8_t *)buffer, WRITE_READ_ADDR, sizeof(buffer)-1);
    if(memcmp(magic, buffer, strlen(magic))!=0){
        return "";
    }
    return buffer+strlen(magic);
}

int saveMnemonic(string mnemonic){
    qspi.Erase_Block(WRITE_READ_ADDR);
    qspi.Write((uint8_t *)magic, WRITE_READ_ADDR, strlen(magic));
    qspi.Write((uint8_t *)mnemonic.c_str(), WRITE_READ_ADDR+strlen(magic), mnemonic.length()+1);
    return 1;
}

int wipe(){
    qspi.Erase_Block(WRITE_READ_ADDR);
    NVIC_SystemReset(); // reset the board
}

/*********** random number generator ***********/

int initRNG(){
  __HAL_RCC_RNG_CLK_ENABLE();
  rng.Instance = RNG;
  if(HAL_RNG_Init(&rng) != HAL_OK){
    return 0;
  }
  return 1;
}

uint32_t getRandomNumber(void){
  uint32_t rnd = 0xff;
  if(HAL_RNG_GenerateRandomNumber(&rng, &rnd) != HAL_OK){
    return 0;
  }
  return rnd;
}

size_t getRandomBuffer(uint8_t * arr, size_t len){
    for(unsigned int i=0; 4*i<len; i++){
        uint32_t r = getRandomNumber();
        if( i*4+1< len ){
            memcpy(arr+i*4, &r, 4);
        }else{
            memcpy(arr+i*4, &r, len-i*4);
        }
    }
    return len;
}

