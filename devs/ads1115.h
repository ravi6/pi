// www.bristolwatch.com
// http://www.bristolwatch.com/rpi/ads1115.html
// Heavily Modified by Ravi Saripalli 

#ifndef ADS115_H
#define ADS115_H

#define CONV_REG    0     // Conversion Register Address
#define CONFIG_REG  1     // Config Register Address

#define OS          1     // Start one Shot
#define MODE        1     // 0 Continuous , 1 One Shot
#define PGA         1   
#define DR          4     // 125 SPS
#define COMPTOR  (0x05)   // Comparator Setup Bits (default)

float AtoD(int ch);   // Single Ended Signals 

#endif
