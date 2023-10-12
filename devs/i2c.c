// I2C bus interface routines
// Ravi Saripalli 3/9/23
#include "i2c.h"

struct I2C {
      int fd         ;  // file discriptor for bus 
      bool isOpen    ;  // status of bus
      uint8_t slave  ;  // slave address 
} i2c ;

void I2C_Open() {
    // open device on /dev/i2c-1 
    if (i2c.isOpen) return ;  
    i2c.fd = open("/dev/i2c-1", O_RDWR) ;
    if (i2c.fd < 0) {
       printf("I2C bus open failed! \n");
       i2c.isOpen = false ;
       exit (1);
    }
    i2c.isOpen = true ;
    printf("Opened I2C bus\n") ;
}
void I2C_Close() {
   close(i2c.fd) ;
    printf("Closed I2C bus\n") ;
}

void I2C_Write(uint8_t *buf, int len) { 
  int lenw =   write(i2c.fd, buf, len) ;
   if (lenw != len) {
      printf("I2C Write Error \n ") ;
      exit(-1);
  }
}

void I2C_Read(uint8_t *buf, int len) { 
   if (read(i2c.fd, buf, len) != len) {
      perror("I2C Read Error\n ") ;
      exit(-1);
  }
}

void I2C_setSlave(uint8_t address) {
  if (ioctl(i2c.fd, I2C_SLAVE, (long)address) < 0) {
    printf("No Slave at %x \n", address);
    exit (1);
  }
   i2c.slave = address ;
}

