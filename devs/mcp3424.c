// Author: Ravi Saripalli
//    5th October 2023
//  Four Channel (single ended) A/D converter
//  12 bit resolution at 240 SPS data rate

#include "common.h"
#include "mcp3424.h"
 
void MCP3424_reset() {
     uint8_t buf[2];  // buffer to send or receive data on bus
     buf[0]  = 0x00 ; 
     buf[1] = 0x06 ;  // Calls Reset
     I2C_Write(&buf, 2) ; 
     buf[0]  = 0x00 ; 
     buf[1] = 0x08 ;  // Calls Latch
     I2C_Write(&buf, 2) ;
}

float MCP3424_AtoD(uint8_t ch) {

// Data Rate (Samples per Second) (bit 3:2)
//  NOTE:  ***   We use 240 sps and we have 12bit resolution  ***
//      This code won't cover higher resoltions 
//       So we have  two bytes of converted data and one byte of config
//       that is returned when read

  uint8_t buf[3];  // buffer to send or receive data on bus
  float volts ;
  const  uint8_t CONFIG_BYTE  \
        = (MCP3424_OS << 7) | ((ch-1) << 5) | (MCP3424_MODE << 4) \
           | (MCP3424_DR << 2) | MCP3424_PGA ;   // Config Byte
  // Start Conversion

//  printf("Start Conversion\n") ;
  buf[0] = MCP3424_WRITE_REG ;     // Write Command byte
  buf[1] = CONFIG_BYTE ; 
//  showBuffer(&buf[0], 1) ;
//  showBuffer(&buf[1], 1) ;
          
/* Debug 
  printf("Configuration sent \n") ;
  MCP3424_showConfig(CONFIG_BYTE) ;
  I2C_Write(&buf[0], 1) ;  I2C_Write(&buf[1], 1) ;  
  printf("***************\n");
*/

 
  // wait for conversion complete (MSBit of Config to go low)
  // first two bytes are data and third byte is config
  // when you send read request command
  // So we need to monitor the RDY bit of the third byte
 // printf("Waiting for Data \n") ;
  int j ; j = 0 ;
  do {
     // Send Read Data Command 
     // Config byte also needed to select the channel to read
      buf[0] = MCP3424_READ_REG ;   
      buf[1] = CONFIG_BYTE ; 
           
      I2C_Write(&buf[0], 2) ;
      I2C_Read(&buf[0], 3) ;  // read response
      //showBuffer(&buf, 3) ;
      j = j + 1 ;
      if (j > 100) {
           printf("Data Ready Flag Failed to Clear \n") ;
           break ;
      }
  } while (!((buf[2] & 0x80) == 0) ); // Check RDY bit cleared in Config byte

/*
  printf("Configuration in Device:\n") ;
  MCP3424_showConfig(buf[2]) ;
  printf("Inferring Data Bits\n") ;
  printf("******END******\n");
*/
  volts = MCP3424_inferData(buf) ;

  return (volts);
} // end AtoD

void MCP3424_showConfig (uint8_t cfg) { // Human Readable Config
  const int sps[4] ={240, 60, 15, 3.75} ;
  const int gain[4] = {1, 2, 4, 8 }; 
  printf("CFG Byte Pattern: %s\n", byte2bin(cfg)) ;
  printf("Channel: %d,  ", ((cfg & 0x60) >> 5) + 1) ;
  printf(" Mode: %s,  ", ((cfg & 0x10) >> 4) ? "Continuous" : "OneShot") ;
  printf(" Rate (samples/s): %d",  sps[(cfg & 0x0C) >> 2 ]) ;
  printf(" Gain: %d\n", gain[(cfg & 0x03)]) ;
}

float MCP3424_inferData(uint8_t buf[]) 
{ // test routine
  uint16_t count ;  // Digital value of converted Analogue input signal
  float volts;    // Analogue input in volts
  
  // Conversion bit length
  const uint8_t resBits [4] = {12, 14, 16, 18} ;

  // Programmable Gain Options (bit 1:0)
  const float gain[4] = {1, 2, 4, 8 }; 
  const uint8_t mask [3] = {0x07, 0x1F, 0xFF} ;
  uint8_t msb, lsb ; 

  // volt per step (or count) 
  // Note resolution changes with dataRate with this chip
  float VPS = 2.048 / gain[MCP3424_PGA] / pow(2, resBits[MCP3424_DR]-1) ;

// Data comes in with MSB starting at buf[0]
//  Note: We cover only upto 16bit resolutions here 
//  We are removing the sign bit along with excess bits
  
  msb = buf[0] & mask[MCP3424_DR] ; 
  lsb = buf[1]  ; 
  // printf("msb lsb %x   %x \n", msb, lsb) ;
  count = (msb << 8) | lsb  ;

  volts =  count * VPS ;
//  printf("Count  %x    %d   volts = %6.3f \n ", count, count, volts) ;
  return (volts) ;
}

