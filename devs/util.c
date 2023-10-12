#include "common.h"

char* byte2bin (uint8_t byte) 
{
   int j ;
   char *cp = malloc(11 * sizeof(uint8_t)) ;
   sprintf(cp, "0b");
   j = 2 ;
   for (int i = 7  ; i >= 0 ; i--) {
       if (byte & (1<<i) ) *(cp+j) = '1' ;
       else *(cp+j) = '0' ;
       j = j + 1 ;
   }
   *(cp+10) ='\0' ;
   return (cp) ;
} // end byte2bin
