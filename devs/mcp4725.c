/*
# Author: Ravi Saripalli
#  A simple D/A driver
*/

#include "common.h"
#include "mcp4725.h"

// Read DAC Register

void MCP4725_DtoA(float volts) {
   uint16_t  count ; // to be written to DAC register
   uint8_t buf[4] ;   

   // Map output voltage to Digital count (12 bit long integer)
   // Split this count into two bytes for transmission
   count = (uint16_t)(volts * MCP4725_COUNTS / MCP4725_VDD) ;
   buf[0] = MCP4725_WRITE_REG ;
   buf[1] = MCP4725_WRITE_CMD ;
   buf[2] = (count & 0x0FFF) >> 4 ;   // Grab 11-4 bits positions of count
   buf[3] = (count & 0x000F) << 4 ; // get 3-0 bit positions of count an place them in MSB  
   printf("Write: %s,  Cmd: %s \n", byte2bin(buf[0]), byte2bin(buf[1])) ;
   printf("Count = %d,  count %x, byte3: %x, byte4:%x \n", count, count, buf[2], buf[3]) ;
   I2C_Write(&buf[0], 1) ;
   I2C_Write(&buf[1], 3) ;
} // end D2A

void MCP4725_reset() {
     uint8_t buf[2];  // buffer to send or receive data on bus
     buf[0]  = 0x00 ; 
     buf[1] = 0x06 ;  // Calls Reset
     I2C_Write(&buf, 2) ;
     buf[0]  = 0x00 ; 
     buf[1] = 0x09 ;  // Reset Power Down
     I2C_Write(&buf, 2) ;
}
