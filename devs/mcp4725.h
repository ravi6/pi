/*
# Author: Ravi Saripalli
#  A simple D/A driver
*/

#ifndef MCP4725_H
#define MCP4725_H

// Read DAC Register

#define MCP4725_DEV_CODE (0x0C)  // 4 bits long (0b1100)
// A2 and A1 bits are factory set (it appears they are 01 respectively)
// A0 pin tied to ground (use 0x03 if A0 is tied to VDD) 
#define MCP4725_DEV_ADDR (0x02)  // 3 bits long 
#define MCP4725_I2C_ADD  ( (MCP4725_DEV_CODE << 3) | MCP4725_DEV_ADDR ) 

#define MCP4725_WRITE_REG ( MCP4725_DEV_CODE << 4 | MCP4725_DEV_ADDR << 1 )
#define MCP4725_READ_REG ( MCP4725_WRITE_REG  | 0x01 )

#define  MCP4725_WRITE_MODE (0x02)   // 3bits long 0x00 Fast, 0x02 RegOnly, 0x03 Reg&EEPROM 
#define  MCP4725_POWER_MODE (0x00)   // 2bits long, 0x00 Normal, 0x01 ShutDown 

#define  MCP4725_WRITE_CMD ( MCP4725_WRITE_MODE << 5 | MCP4725_POWER_MODE << 1 )

#define MCP4725_VDD  5.0   // Chip VDD voltage 
#define MCP4725_COUNTS 4096  // 12 bit conversion

void MCP4725_DtoA(float volts) ;
#endif

