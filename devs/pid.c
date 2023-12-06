/* Simple PID controller 
   with I2C A/D and D/A converter chips
   and Raspberry Pi
   Author:  Ravi Saripalli
*/

#include "common.h"
#include "i2c.h"
#include "mcp3424.h"
#include "mcp4725.h"
#include <sys/file.h>
#include <math.h>
#include <unistd.h>  // access function
#include "rtable.h"

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
} pid ;

struct table spTable ; // setpoint Table data
const float  dt = 100e-3   ;   // time interval (s)
int ticks ;

float getP() {  // get Measured Variable, Chamber pressure
   float v, vmin, vmax ;
   float p, pmin, pmax ; // pressure in bar (abs)
   const float VDD = 5.2 ;

   vmin = 0.5/2 ;
   vmax = 4.5/2 ;
   pmin = 0 ;
   pmax = 1 ; 
//   I2C_setSlave(MCP3424_I2C_ADD) ;  // Talkto A2D
//   v = MCP3424_AtoD(3) * 2   ;  

//   I2C_setSlave(MCP3424_I2C_ADD) ;  // Talkto A2D
     v=2.2 ; //v = MCP3424_AtoD(3)    ;  
//   p = pmin  + (v - vmin) * (pmax - pmin) / (vmax - vmin) ;
//   p = ( ( v  / 0.196 ) - 0.1 * VDD) / ( 0.8 * VDD);
     // simulated chamber here
     // show how p varies with bleed flow
     p = 0.9 *  pow(pid.op/pid.opMax, 1)  * (1 - exp(-ticks*dt/10)) ; 
     return(p) ;
} // end getP

void setFlow (float f) { // Control output ... bleed flow rate
   float fmin, fmax ; // flow in lpm
   float v, vmin, vmax ; 

   vmin = 0 ; vmax = 5 ;
   fmin = 0 ; fmax = 100 ; 
   v = vmin  + (f - fmin) * (vmax - vmin) / (fmax - fmin) ;  
 //  I2C_setSlave(MCP4725_I2C_ADD) ;  // Talkto D2A
  // MCP4725_DtoA(v) ;
} // end setFlow

void readParams(int wait) {
  // Read pidParams, command  from parrams file
  // don't wait if file is busy and lock it when reading from it
   FILE *fp ;
   float kc, ki, kd, sp ;
   int cmd ;

back:
   fp = fopen("pidParams", "r") ;
   if ((fp == NULL)  && !wait) return ;
   else if ((fp == NULL) && wait) goto back  ;

   int n = fscanf(fp, "%f %f %f",  &kc, &ki, &kd); 
   fclose(fp) ;

   pid.opMin = 0 ;    //flow
   pid.opMax = 100 ;
   pid.mvMin = 0 ;   // pressure
   pid.mvMax = 1 ;
   pid.Kc =  kc *  (pid.opMax - pid.opMin) / (pid.mvMax - pid.mvMin) ;
   pid.Ki = ki ;
   pid.Kd = kd ;
} // end readParams

 
int pidLoop() {

  float err, err_p  ;   // (SP - MV)  in the current and last time step
  float errInt ;  // Integral of Error w.r.t time
  float errIntMax ; // Inegral saturation limit
  float errDer ;  // Time derivative of error
  float Integ ; // integral action term

  ticks = 0 ;
 
  // I2C_Open() ;  // open the bus

  readParams(1) ; // load PID params from params file
		     // wait for ever for the file to be ready

  // Load sp trend table
  spTable = loadTable() ;  // loads sp data table

  // Get setpoint at time zero 
  // Data file should contain t=0 point as the first data pt
  pid.sp = intPolate(spTable, 0) ;

  errInt = 0 ;
  err_p =  pid.sp - getP()  ;  // previous error
  

// keep running as longas run exists
  while (access("run", F_OK) == 0) {
    // Lookup PID parameters every 10 ticks 
    if ((ticks % 10) == 0) {
      readParams(0) ;  
      pid.sp = intPolate(spTable, ticks * dt) ; // sp at current time
    }

     pid.mv = getP() ;
     err = pid.sp - pid.mv ;
     errDer = (err - err_p) / dt ;
     errInt = errInt + (err + err_p) * 0.5 * dt ;
     Integ = errInt / pid.Ki ;
     
     // PID controller expression for its output
     pid.op = pid.Kc * err + Integ + pid.Kd * errDer ; 
     
     // Adjust integral amount by the excess/deficit 
     // of the output from its limiting values 
     // and then Clip final pid output to limits
     if (pid.op > pid.opMax) {
         Integ = Integ - (pid.op - pid.opMax)  ; // reduce heap
         pid.op = pid.opMax ;
     }
     else if(pid.op < pid.opMin) {
         Integ = Integ + (pid.opMin - pid.op) ;  // add to heap
         pid.op = pid.opMin ;
     }
     //printf("mv = %f err= %f op= %f\n", pid.mv, err, pid.op); 
     //printf("errInt = %f err_p= %f \n", errInt, err_p); 
     setFlow(pid.op) ;
     
     if ( (ticks%10) == 0) { // coarsen the plotting
       FILE *fp = fopen("pidData","a") ;
       if (fp != NULL) {
	 fprintf(fp, "%f %f %f %f  %f %f %f \n", 
	     ticks * dt, pid.sp, pid.mv, pid.op,
	     pid.Kc, pid.Ki, pid.Kd) ;
	 fclose(fp) ;
       }
     }

     err_p = err ;    // ready for next loop
     usleep(dt*1e6) ;  // usleep works in microseconds
     ticks = ticks + 1 ;
  } // PID loop end

} // end pidLoop

int main() {
//  sp = 0.8 ; kc = 1.6 ; ki = 0.1 ; kd = 0 ;
  pidLoop();
  remove("run") ;  // remove run flag
  return(0) ;
}
