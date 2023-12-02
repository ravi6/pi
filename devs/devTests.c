#include "common.h"
#include "i2c.h"
#include "mcp3424.h"
#include "mcp4725.h"
// These routines are to debug and test
// A/D and D/A hardware
void testAtoD() {

// Test AtoD 
     I2C_Open() ;  // open the bus
     I2C_setSlave(MCP3424_I2C_ADD) ;  // Talkto A2D
     printf("Using Device Add %x \n", MCP3424_I2C_ADD);
     MCP3424_reset() ;
     int channel = 3 ;
     printf("Read Channel %d\n", channel) ;
     float vin ;
     vin = MCP3424_AtoD(channel) ; // read from ch0
     printf("Channel:%d Voltage:  %6.3f \n", channel, vin) ;
     float bar ;
     float VDD ;
     VDD = 5.2 ;
     bar = ( ( vin  / 0.196 ) - 0.1 * VDD) / ( 0.8 * VDD);
     printf("Pressure in Bar : %f \n ", bar) ;
}

void testDtoA() {
// Test DtoA 
     I2C_Open() ;  // open the bus
     I2C_setSlave(MCP4725_I2C_ADD) ;  // Talkto A2D
     printf("Using MCP4725 Device Add %x \n", MCP4725_I2C_ADD);
 //    MCP4725_reset() ;
     float vout ;
     printf("Enter Volts:  ");
     scanf("%f", &vout) ;
     printf("Vout should be %f\n", vout);
     MCP4725_DtoA(vout) ; // 
}
int testAll() {
     I2C_Open() ;  // open the bus
     I2C_setSlave(MCP4725_I2C_ADD) ;  // Talkto D2A
     printf("Using MCP4725 Device Add %x \n", MCP4725_I2C_ADD);
     MCP4725_reset() ;

     I2C_setSlave(MCP3424_I2C_ADD) ;  // Talkto A2D
     printf("Using MCP3424 Device Add %x \n", MCP3424_I2C_ADD);
     MCP3424_reset() ;

     float x[6] = {0.111, 0.222, 0.333, 0.444, 0.555, 5.0} ;
     float vout ;
     for (int j=0 ; j < 6 ; j++) {
       I2C_setSlave(MCP4725_I2C_ADD) ;  // Talkto D2A
       vout = x[j] ;
       I2C_setSlave(MCP4725_I2C_ADD) ;  
       MCP4725_DtoA(vout) ; // 
       int channel = 3 ;
       float vin ;
       I2C_setSlave(MCP3424_I2C_ADD) ;  // Talkto A2D
       vin = MCP3424_AtoD(channel) ; // read from ch0
       printf("Channel:%d Voltage:  %6.3f  %6.3f \n",  channel, vin, vout) ;
    }
}
