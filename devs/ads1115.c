// Adapted from
// http://www.bristolwatch.com/rpi/ads1115.html
// Heavily Modified by Ravi Saripalli 

#include "common.h"
#include "ads1115.h"

// Programmable Gain Options (bit 11:9)
const float gain[6] = { 6.144,  4.096,  2.048,  1.024, 0.512,  0.256 }; 

// Data Rate (Samples per Second)
const int sps[8] ={8, 16, 32, 64, 128, 250, 475, 860} ;

 
float AtoD(int ch) {

	uint16_t count ;  // Digital value of converted Analogue input signal
	float volts;    // Analogue input in volts
	uint8_t buf[3];  // buffer to send or receive data on bus

	//ADS1115 is a 16 bit A/D (15 bit if Single Ended ) 
	//So, volt per step (or count) 
	const float VPS = gain[PGA] / pow(2, 15) ;
                            
	// set config register and start conversion
	  buf[0] = CONFIG_REG ;
          uint8_t MUX = 0x04 + (uint8_t)ch ;   //  ch 0 to 3 
	  buf[1] = MODE | (PGA << 1) | (MUX << 3) | (OS << 3) ;
	  buf[2] = COMPTOR | ((DR << 5)) ;

          I2C_Write(&buf[0], 3) ;  // begin converson

	  // wait for conversion complete (bit 15 high)
	  do {
            I2C_Read(&buf[0], 2) ;
	  } while ((buf[0] & 0x80) == 0);

	  // write conversion register pointer first
	  buf[0] = CONV_REG;   // conversion register is 0
          I2C_Write(&buf[0], 1) ;
          I2C_Read(&buf[0], 2) ;  // read conveted value bytes

	  // Read the digital count of analogue input
          // and scale it to volts
	  count = buf[0] << 8 | buf[1];
	  if (count < 0)   count = 0;
	  volts = count * VPS; // convert to voltage
	  printf("count: DEC %d,  Volts: %4.3f \n",
		  count, volts);
	  return (volts);
} // end AtoD
