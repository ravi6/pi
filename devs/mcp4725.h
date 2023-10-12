/*
# Author: Ravi Saripalli
#  A simple D/A driver
*/

#ifndef MCP4725_H
#define MCP4725_H

// Read DAC Register

#define DEV_CODE (0x0C)  // 4 bits long
// A0 pin tied to ground (use 0x07 if A0 is tied to VDD) 
#define DEV_ADDR (0x06)  // 3 bits long 

#define WRITE_REG ( DEV_CODE << 4 | DEV_ADDR << 1 )
#define READ_REG ( WRITE_REG  | 0x01 )

#define  WRITE_MODE (0x02)   // 3bits long 0x00 Fast, 0x02 RegOnly, 0x03 Reg&EEPROM 
#define  POWER_MODE (0x00)   // 2bits long, 0x00 Normal, 0x01 ShutDown 

#define  WRITE_CMD ( WRITE_MODE << 5 | POWER_MODE << 1 )

#define VDD  3.3   // Chip VDD voltage 
#define COUNTS 4096  // 12 bit conversion

void DtoA(float volts) ;
#endif

