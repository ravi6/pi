// I2C interface header file
// Ravi Saripalli
//
#ifndef I2C_H
#define I2C_H
#include "common.h"
void I2C_Open() ;
void I2C_Close() ;
void I2C_Read(uint8_t *buf, int len) ;
void I2C_Write(uint8_t *buf, int len) ;
void I2C_setSlave(uint8_t address) ;
#endif
