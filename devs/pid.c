// Simple PID controller 
//   with I2C A/D and D/A converter chips
//   and Raspberry Pi
//   Author:  Ravi Saripalli
#include "common.h"
#include "i2c.h"
#include "mcp3424.h"
#include "mcp4725.h"
#include <sys/file.h>

struct PID {
  float mv     ; // Measured Value 
  float op     ; //  Controller Output Value  
  float sp     ; //  Controller Setpoint
  float Kc     ; // Gain
  float Ki     ; // Integral action (s)
  float Kd     ; // Derivative action 
  float mvMin  ; 
  float mvMax  ; // Measured value (pressure) limits
  float opMin  ;
  float opMax  ;  // Output (flow setpoint) limits
  int   cmd    ; // 1 start, 0 stop
  int   state  ; // 1 running, 0 stopped
} pid ;

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

void readParams(int wait) {
  // Read pidParams, command  from parrams file
  // don't wait if file is busy and lock it when reading from it
   FILE *fp ;

back:
   fp = fopen("pidParams", "r") ;
   if ((fp == NULL)  && !wait) return ;
   else if ((fp == NULL) && wait) goto back  ;

   float kc, ki, kd ;
   int cmd ;
   int n = fscanf(fp, "%d %f %f %f", &cmd, &kc, &ki, &kd); 
   fclose(fp) ;

   pid.opMin = 0 ;    //flow
   pid.opMax = 100 ;
   pid.mvMin = 0 ;   // pressure
   pid.mvMax = 1 ;
   pid.Kc =  kc *  (pid.opMax - pid.opMin) / (pid.mvMax - pid.mvMin) ;
   pid.Ki = ki ;
   pid.Kd = kd ;
   pid.cmd = cmd ;
} // end readParams

int pidLoop() {

  float err, err_p  ;   // (SP - MV)  in the current and last time step
  float errInt ;  // Integral of Error w.r.t time
  float errIntMax ; // Inegral saturation limit
  float errDer ;  // Time derivative of error

  const float  dt = 100e-3   ;   // time interval (s)
  int ticks = 0 ;
 
  I2C_Open() ;  // open the bus

  // Keep monitoring params until pid is started
  do { readParams(1) ; // load PID params from params file
		     // wait for ever for the file to be ready
  } while (pid.cmd == 0) ;

  errInt = 0 ;
  err_p =  pid.sp - getP()  ;  // previous error
  
  while(pid.cmd == 1) {
    if ((ticks%100) == 0)
     readParams(0) ;  // don't wait if you can't get access to file
     pid.mv = getP() ;
     err = pid.sp - pid.mv ;
     errDer = (err - err_p) / dt ;
     errInt = errInt + (err + err_p) * 0.5 * dt ;

     //Stop Integral Windup
     if (errInt>pid.opMax*pid.Ki)  		  
        errInt=pid.opMax*pid.Ki;
     else if (errInt<pid.opMin*pid.Ki)
        errInt=pid.opMin*pid.Ki;
       
     // PID controller expression for its output
     pid.op = pid.Kc * err + errInt / pid.Ki + pid.Kd * errDer ; 
     // Stop Integral windup
     if (pid.op > pid.opMax) 
         pid.op = pid.opMax ;
     else if (pid.op < 0)
         pid.op = 0 ;
     //printf("mv = %f err= %f op= %f\n", pid.mv, err, pid.op); 
     //printf("errInt = %f err_p= %f \n", errInt, err_p); 
     setFlow(pid.op) ;

     
     FILE *fp = fopen("pidData","rw") ;
     if (fp != NULL) {
       printf("%d %f  %f %f \n", ticks, pid.sp, pid.mv, pid.op) ;
       fclose(fp) ;
     }

     err_p = err ;    // ready for next loop
     usleep(dt*1e6) ;  // usleep works in microseconds
     ticks = ticks + 1 ;
  } // PID loop end

  pid.state = 0 ;
} // end PID controller  loop

int main(){
  float sp, kc, ki, kd ;
 //  sp = 0.8 ; kc = 1.6 ; ki = 0.1 ; kd = 0 ;    
  pidLoop(sp, kc, ki, kd) ;
}
