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
   //return ( MCP3424_AtoD(1) ) ;    // using Ch1 for this 
   // Fixed set point for now
   float setpt ;
   setpt = 0.5 ;   //  pressure in bar  
   return (setpt);
} // end getSetPt

float getP() {
   float v, vmin, vmax ;
   float p, pmin, pmax ; // pressure in bar (abs)
/*
   vmin = 0.5/2 ;
   vmax = 4.5/2 ;
   pmin = 0 ;
   pmax = 1 ; 
   I2C_setSlave(MCP3424_I2C_ADD) ;  // Talkto A2D
   v = MCP3424_AtoD(3) * 2   ;  
*/
   I2C_setSlave(MCP3424_I2C_ADD) ;  // Talkto A2D
   v = MCP3424_AtoD(3)    ;  
   p = pmin  + (v - vmin) * (pmax - pmin) / (vmax - vmin) ;  
     float VDD ;
     VDD = 5.2 ;
     p = ( ( v  / 0.196 ) - 0.1 * VDD) / ( 0.8 * VDD);
}

void setFlow (float f) {
   float fmin, fmax ; // flow in lpm
   float v, vmin, vmax ; 
   vmin = 0 ;
   vmax = 5 ;
   fmin = 0 ;
   fmax = 100 ; 
   v = vmin  + (f - fmin) * (vmax - vmin) / (fmax - fmin) ;  
   I2C_setSlave(MCP4725_I2C_ADD) ;  // Talkto D2A
   MCP4725_DtoA(v) ;
}

int pidLoop(float sp, float kc, float ki, float kd) {

  float err, err_p  ;   // (SP - MV)  in the current and last time step
  float errInt ;  // Integral of Error w.r.t time
  float errIntMax ; // Inegral saturation limit
  float errDer ;  // Time derivative of error

  const float  dt = 100e-3   ;   // time interval (s)
  pid.opMin = 0 ;    //flow
  pid.opMax = 100 ;
  pid.mvMin = 0 ;   // pressure
  pid.mvMax = 1 ;
  pid.Kc =  kc *  (pid.opMax - pid.opMin) / (pid.mvMax - pid.mvMin) ;
  pid.Ki = ki ;
  pid.Kd = kd ;

  FILE* fd ;
 
  I2C_Open() ;  // open the bus

  errInt = 0 ;
  //err_p =  getSetPt() - getP()  ;  // previous error
  err_p =  sp - getP()  ;  // previous error

  while(1) {
  fd = fopen("junk", "a") ;
     pid.mv = getP() ;
     pid.sp = sp ;   //getSetPt() ;
     err = pid.sp - pid.mv ;
     errDer = (err - err_p) / dt ;
     errInt = errInt + (err + err_p) * 0.5 * dt ;
     
     // PID controller expression for its output
     pid.op = pid.Kc * err + errInt / pid.Ki + pid.Kd * errDer ; 
     // Stop Integral windup
     if (pid.op > pid.opMax) 
         pid.op = pid.opMax ;
     else if (pid.op < 0)
         pid.op = 0 ;
     //printf("mv = %f err= %f op= %f\n", pid.mv, err, pid.op); 
     //printf("errInt = %f err_p= %f \n", errInt, err_p); 
     fprintf(fd, "%f  %f %f \n", pid.sp, pid.mv, pid.op) ;
     
     setFlow(pid.op) ;
     err_p = err ;    // ready for next loop
     usleep(dt*1e6) ;  // usleep works in microseconds
     fclose(fd);
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
int main(){
  float sp, kc, ki, kd ;
   sp = 0.5 ;
   kc = 1.6 ;
   ki = 0.1 ;
   kd = 0 ;    
//   testDtoA() ;
//   testAtoD() ;
 //    testAll() ;
    pidLoop(sp, kc, ki, kd) ;
}
