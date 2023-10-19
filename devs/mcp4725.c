/*
# Author: Ravi Saripalli
#  A simple D/A driver
*/

#include "common.h"
#include "mcp4725.h"

// Read DAC Register

void MCP4725_DtoA(float volts) {
   uint16_t  count ; // to be written to DAC register
   uint8_t lsb, msb ;   // least and most significant bytes

   // Map output voltage to Digital count (12 bit long integer)
   // Split this count into two bytes for transmission
   count = (uint16_t)(volts * MCP4725_COUNTS / MCP4725_VDD) ;
   msb = (count >> 8) & 0x000F;   // Grab 4 most significat bits of 12
   lsb = count & 0x00FF ; // get last eight bits
   printf("%d, hex follows   %x, %x, %x \n", count, count, msb, lsb) ;
   uint8_t buf[] = {MCP4725_WRITE_REG,  MCP4725_WRITE_CMD, msb, lsb} ;
   I2C_Write(&buf[0], 4) ;
} // end D2A
