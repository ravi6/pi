// Author: Ravi Saripalli
//    5th October 2023
//  Four Channel (single ended) A/D converter
//  12 bit resolution at 280 SPS data rate
//  18 bit resolution at 6 SPS data rate

#ifndef MCP3424_H
#define MCP3424_H

// We always specify all bit patterns from lsb to msb
//  and set remaining MSBs to zero

#define MCP3424_DEV_CODE    0x0D  // Device Code bits 1101 (unchangeable)
#define MCP3424_DEV_ADDR    0x02  // Three bit Device Address (settable)
                                  // maximum 3 bits
                                  // We chose A2 A1 A0 as (0 1 0)
// for the CoreElectronic supplied chip
//    dips switches are (On,On) .... that gives device address of 6E
//      0 1101 110   ---> so in hex mode   0110 1110 -> 6E
// That is waht exactly you see on the chip
// The A/D chip we got from CoreElectronics is based on mcp3424.
//
// The format of device address (as seen by I2C bus can be created as
// follows.  This holds good for any device on I2C bus.  
// Fill in a byte as following
//       0 DC3 DC2 DC1 DC0 A2 A1 A0
//   The aboe gives you the I2C bus Address 
//   DC stands for Device Code Bits, A stands for Address bits



#define MCP3424_WRITE_REG   ( (MCP3424_DEV_CODE << 4) | (MCP3424_DEV_ADDR << 1 ) | 0 ) 
#define MCP3424_READ_REG    ( (MCP3424_DEV_CODE << 4) | (MCP3424_DEV_ADDR << 1 ) | 1 ) 
#define MCP3424_I2C_ADD  ( (MCP3424_DEV_CODE << 3) | MCP3424_DEV_ADDR ) 

#define MCP3424_OS          1     // Start Conversion (in continuous  Mode)
#define MCP3424_MODE        1     // 1 Continuous , 0 One Shot 
#define MCP3424_PGA         0     // Programmable Gain (gain=1)
#define MCP3424_DR          1     // 60 SPS (sample rates)
                                  // 14 bit resolution

float MCP3424_AtoD(uint8_t ch);   // Single Ended Signals 
void  MCP3424_showConfig() ;      //  See device configure
float inferData(uint8_t buf[]) ;
#endif
