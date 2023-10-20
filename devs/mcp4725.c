/*
# Author: Ravi Saripalli
#  A simple D/A driver
*/

#include "common.h"
#include "mcp4725.h"

// Read DAC Register

void MCP4725_DtoA(float volts) {
   uint16_t  count ; // to be written to DAC register
   uint8_t byte3, byte4 ;   // least and most significant bytes

   // Map output voltage to Digital count (12 bit long integer)
   // Split this count into two bytes for transmission
   count = (uint16_t)(volts * MCP4725_COUNTS / MCP4725_VDD) ;
   byte3 = (count & 0x0FFF) >> 4 ;   // Grab 11-4 bits positions of count
   byte4 = (count & 0x000F) << 4 ; // get 3-0 bit positions of count an place them in MSB  
   printf("Write: %s,  Cmd: %s \n", byte2bin(MCP4725_WRITE_REG), byte2bin(MCP4725_WRITE_CMD)) ;
   printf("Count = %d,  count %x, byte3: %x, byte4:%x \n", count, count, byte3, byte4) ;
   uint8_t buf[] = {MCP4725_WRITE_REG,  MCP4725_WRITE_CMD, byte3, byte4} ;
   I2C_Write(&buf[0], 4) ;
} // end D2A
