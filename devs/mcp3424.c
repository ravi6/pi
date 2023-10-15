// Author: Ravi Saripalli
//    5th October 2023
//  Four Channel (single ended) A/D converter
//  12 bit resolution at 280 SPS data rate
//  18 bit resolution at 6 SPS data rate

#include "common.h"
#include "mcp3424.h"
 
float MCP3424_AtoD(uint8_t ch) {

// Data Rate (Samples per Second) (bit 3:2)
//  const int sps[4] ={240, 60, 15, 3.75} ;
//  NOTE:  ***   We use 240 sps and we have 12bit resolution  ***
//      This code won't cover higher resoltions 
//       So we have one two bytes of converted data and one byte of config

  uint8_t buf[2];  // buffer to send or receive data on bus
  float volts ;
                            
  // Start Conversion
  printf("Start Conversion\n") ;
  buf[0] = MCP3424_WRITE_REG ;     // Write Command byte
  buf[1] = (MCP3424_OS << 7) | (ch << 6) | (MCP3424_MODE << 4) \
           | (MCP3424_DR << 3) | MCP3424_PGA ;   // Config Byte
  inferData(buf) ;
  I2C_Write(&buf[0], 2) ;  
  inferData(buf) ;
  printf("***************\n");

 
  // wait for conversion complete (bit 15 high)
  // first two bytes are data and third byte is config
  // when you send read request command
  // So we need to monitor the RDY bit of the third byte
  printf("Waiting for Data \n") ;
  int j ; j = 0 ;
  do {
     // Send Read Data Command 
     // Config byte also needed to select the channel to read
      buf[0] = MCP3424_READ_REG;   
      buf[1] =  (ch << 6) | (MCP3424_MODE << 4) \
           | (MCP3424_DR << 3) | MCP3424_PGA ;   // Config Byte
      I2C_Write(&buf[0], 2) ;

      I2C_Read(&buf[0], 3) ;  // read response
      printf (" %d  %s : %s : %s     \n",  j, 
       byte2bin(buf[0]), byte2bin(buf[1]), 
       byte2bin(buf[2])  ) ; 
      j = j + 1 ;
      if (j > 1000) {
           printf("Data Ready Flag Failed to Clear \n") ;
           break ;
      }
  } while (!((buf[2] & 0x80) == 0) ); 

  inferData(buf) ;
  printf("***************\n");
  
  // write conversion register pointer first
  printf("Data loading  \n") ;
  buf[0] = MCP3424_READ_REG;   
  I2C_Write(&buf[0], 1) ;
  I2C_Read(&buf[0], 3) ;  // read conveted value bytes
  volts = inferData(buf) ;
  return (volts);
} // end AtoD

void MCP3424_showConfig() { // see what the device config is
  uint8_t buf[3];  // buffer to send or receive data on bus
  buf[0] = MCP3424_READ_REG;   // conversion register is 0
  I2C_Write(&buf[0], 1) ;
  I2C_Read(&buf[0], 3) ;  // read conveted value bytes
  inferData(buf) ;
}

float inferData(uint8_t buf[]) 
{ // test routine
  uint16_t count ;  // Digital value of converted Analogue input signal
  float volts;    // Analogue input in volts
  
  // Conversion bit length
  const uint8_t resBits [4] = {12, 14, 16, 18} ;

  // Programmable Gain Options (bit 1:0)
  const float gain[4] = {1, 2, 4, 8 }; 

  // volt per step (or count) 
  // Note resolution changes with dataRate with this chip
  float VPS = 2.048 / gain[MCP3424_PGA] / pow(2, resBits[MCP3424_DR]-1) ;

  printf("%s : %s : %s  :  \n", byte2bin(buf[0]), byte2bin(buf[1]), byte2bin(buf[2])  ) ; 
//  printf("%x : %x : %x  :  \n", (buf[0]), (buf[1]), (buf[2])  ) ; 

  // Grab least significan 11 bits of [buf0][buf1] bit pattern
  count = ((buf[0] & 0x07) << 4 ) | buf[1] ; 
  volts = count * VPS ;
  printf("Count  %x    %d   volts = %6.3f \n ", count, count, volts) ;
  return (volts) ;
}

