// Author: Ravi Saripalli
//    5th October 2023
//  Four Channel (single ended) A/D converter
//  12 bit resolution at 280 SPS data rate
//  18 bit resolution at 6 SPS data rate

#include "common.h"
#include "mcp3424.h"
 
float MCP3424_AtoD(uint8_t ch) {

  // Programmable Gain Options (bit 1:0)
  const float gain[4] = {1, 2, 4, 8 }; 

  // Data Rate (Samples per Second) (bit 3:2)
//  const int sps[4] ={240, 60, 15, 3.75} ;

  // Conversion bit length
  const uint8_t resBits [4] = {12, 14, 16, 18} ;

  uint16_t count ;  // Digital value of converted Analogue input signal
  float volts;    // Analogue input in volts
  uint8_t buf[3];  // buffer to send or receive data on bus

  //So, volt per step (or count) 
  // Note resolution changes with dataRate with this chip
  float VPS = gain[MCP3424_PGA] / pow(2, resBits[MCP3424_DR]) ;
                            
  printf("Start Conversion\n") ;
// Start Conversion
  buf[0] = MCP3424_WRITE_REG ;
  buf[1] = (MCP3424_OS << 7) | (ch << 6) | (MCP3424_MODE << 4) \
           | (MCP3424_DR << 3) | MCP3424_PGA ;
  printf("%s : %s : %s\n", byte2bin(buf[0]), byte2bin(buf[1]), byte2bin(buf[2])  ) ; 
  I2C_Write(&buf[0], 2) ;  
  printf("%s : %s : %s\n", byte2bin(buf[0]), byte2bin(buf[1]), byte2bin(buf[2])  ) ; 
 
 printf("Waiting for Data \n") ;
 // wait for conversion complete (bit 15 high)
  do {
    I2C_Read(&buf[0], 2) ;
    usleep(1000) ;
  } while ((buf[0] & 0x80) == 0);

  printf("Awake  %s : %s : %s\n", byte2bin(buf[0]), byte2bin(buf[1]), byte2bin(buf[2])  ) ; 
 printf("Data loading  \n") ;
  // write conversion register pointer first
  buf[0] = MCP3424_READ_REG;   // conversion register is 0
  I2C_Write(&buf[0], 1) ;
  printf("%s : %s : %s\n", byte2bin(buf[0]), byte2bin(buf[1]), byte2bin(buf[2])  ) ; 
  I2C_Read(&buf[0], 2) ;  // read conveted value bytes
  printf("%s : %s : %s\n", byte2bin(buf[0]), byte2bin(buf[1]), byte2bin(buf[2])  ) ; 
   

  // Read the digital count of analogue input
  // and scale it to volts
  count = buf[0] << 8 | buf[1];
  if (count < 0)   count = 0;
  volts = count * VPS; // convert to voltage
  printf("count: DEC %d,  Volts: %4.3f \n",
	  count, volts);
  return (volts);
} // end AtoD

void MCP3424_showConfig() { // see what the device config is
  uint8_t buf[3];  // buffer to send or receive data on bus
  buf[0] = MCP3424_READ_REG;   // conversion register is 0
  I2C_Write(&buf[0], 1) ;
  I2C_Read(&buf[0], 3) ;  // read conveted value bytes
  printf("%s : %s : %s  :  \n", byte2bin(buf[0]), byte2bin(buf[1]), byte2bin(buf[2])  ) ; 
  printf("%x : %x : %x  :  \n", (buf[0]), (buf[1]), (buf[2])  ) ; 
}

