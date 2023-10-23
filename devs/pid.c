// Simple PID controller 
//   with I2C A/D and D/A converter chips
//   and Raspberry Pi
//   Author:  Ravi Saripalli
#include "common.h"
#include "i2c.h"
#include "mcp3424.h"
#include "mcp4725.h"

struct PID {
  float mv     ; // Measured Value 
  float op     ; //  Controller Output Value  
  float sp     ; //  Controller Setpoint
  float Kc     ; // Gain
  float Ki     ; // Integral action (s)
  float Kd     ; // Derivative action 
  float mvMax  ; // Measured value limits
  float mvMin  ;
  float opMax  ; // Output limits
  float opMin  ;
} pid ;

float getSetPt() { // let us use another anlog signal for this 
   return ( MCP3424_AtoD(1) ) ;    // using Ch1 for this 
} // end getSetPt

int pidLoop() {

  float err, err_p  ;   // (SP - MV)  in the current and last time step
  float errInt ;  // Integral of Error w.r.t time
  float errIntMax ; // Inegral saturation limit
  float errDer ;  // Time derivative of error

  errIntMax = pid.opMax / pid.Ki ;  // A reasonable value
 
  const int  dt = 100e-4   ;   // time interval (s)
   
  I2C_Open() ;  // open the bus
  I2C_setSlave(MCP3424_I2C_ADD) ;  // Talkto A2D

  errInt = 0 ;
  err_p =  getSetPt() - MCP3424_AtoD(0)  ;  // previous error

  while(1) {
     pid.mv = MCP3424_AtoD(0) ;
     pid.sp = getSetPt() ;
     err = pid.sp - pid.mv ;
     errDer = (err - err_p) / dt ;
     errInt = errInt + (err + err_p) * 0.5 * dt ;
     if (errInt >= errIntMax) errInt = errIntMax ; // Stop integral windup 
     
     // PID controller expression for its output
     pid.op = pid.Kc * err + errInt / pid.Ki + pid.Kd * errDer ; 
     MCP4725_DtoA(pid.op) ;
     err_p = err ;    // ready for next loop
     usleep(dt*1e6) ;  // usleep works in microseconds
  } // PID loop end

} // end PID controller  loop

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

     float x[5] = {0.111, 0.222, 0.333, 0.444, 0.555} ;
     float vout ;
     for (int j=0 ; j < 5 ; j++) {
       I2C_setSlave(MCP4725_I2C_ADD) ;  // Talkto D2A
       vout = x[j] ;
       usleep(2e6) ;
       I2C_setSlave(MCP4725_I2C_ADD) ;  
       MCP4725_DtoA(vout) ; // 
       int channel = 3 ;
       float vin ;
       I2C_setSlave(MCP3424_I2C_ADD) ;  // Talkto A2D
       vin = MCP3424_AtoD(channel) ; // read from ch0
       printf("Channel:%d Voltage:  %6.3f  %6.3f \n",  channel, vin, vout) ;
    }
}
int main(){
 //  testDtoA() ;
  // testAtoD() ;
     testAll() ;
}
