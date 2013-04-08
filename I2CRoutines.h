/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __I2CROUTINES_H
#define __I2CROUTINES_H

#include "stm32f10x.h"
#include "text_buffer.h"

#include <stdbool.h>

#define I2CClockSpeed 100000
// #define I2CClockSpeed 10000
#define OwnAddress1 0x32

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    Error,
    Success
} I2CStatus;

I2CStatus I2C_Master_Read( const unsigned char NumByteToRead, unsigned char SlaveAddress );
I2CStatus I2C_Master_Write( const unsigned char* message, const unsigned char length, unsigned char SlaveAddress );
void I2C_Slave_BufferReadWrite(void);
void I2C_LowLevel_Init(void);
bool I2C_IsBusy(void);

TextBuffer* I2C_GetReadingBuffer(void);

#ifdef __cplusplus
}
#endif

#endif
