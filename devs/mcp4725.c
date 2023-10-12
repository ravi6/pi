/*
# Author: Ravi Saripalli
#  A simple D/A driver
*/

#include "common.h"
#include "mcp4725.h"

// Read DAC Register

void DtoA(float volts) {
   uint16_t  count ; // to be written to DAC register
   uint8_t lsbData, msbData ;   // least and most significant bytes

   // Map output voltage to Digital count (12 bit long integer)
   // Split this count into two bytes for transmission
   count = (uint16_t)(volts * COUNTS / VDD) ;
   msbData = count >> 4 ;   // Grab First 8 bits 
   lsbData = count & 0x000F ; // get last four bits

   uint8_t buf[] = {WRITE_REG,  WRITE_CMD, msbData, lsbData} ;
   I2C_Write(&buf[0], 4) ;
} // end D2A
