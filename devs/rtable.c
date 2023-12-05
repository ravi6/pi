#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct table {
    int n ; //  number of entries
    float* x ;
    float* y ;
} ;

float intPolate(struct table tbl, float xi) {
 // assume x array is montonically increasing values
 //  no duplicate x entries 
   
  if (xi <= tbl.x[0]) return(tbl.y[0]) ;
  if (xi >= tbl.x[tbl.n-1]) return(tbl.y[tbl.n-1]) ;

  for (int i =0 ;  i < tbl.n-1 ; i++) {
      if ( (xi >= tbl.x[i]) && (xi <= tbl.x[i+1]) ) {
	float yi = tbl.y[i] + (xi - tbl.x[i]) 
	            * (tbl.y[i+1] - tbl.y[i]) / (tbl.x[i+1] - tbl.x[i]) ;
	return(yi) ;
      }
  }

} // end intPolate

int getLineCount(FILE* fp) {
   int c;
   int n = 0;
   do {
      c = fgetc(fp);
      if (c == '\n') n = n + 1 ;
   } while(! feof(fp));
   rewind(fp) ;  
   return(n);
}

struct table loadTable () {
  // Loads x,y table data (space separated)
  // from given file 
  // Assume data file contains just tabled data
  // A crude data table loading without any error
  // trapping if "spData" file does not conform to the format

  FILE *fp;

  fp = fopen("spData","rw");
  if(fp == NULL) {
     perror("Error in opening spData file");
  }

  int n = getLineCount(fp) ;
  float *x = (float*)malloc(n*sizeof(float)) ;
  float *y = (float*)malloc(n*sizeof(float)) ;
  //printf("Loading Data Table\nidx,  x,  y \n") ;
  for (int i=0 ; i < n ; i++) {
     int rc = fscanf(fp, "%f %f", &x[i], &y[i]) ;
   //  printf("%d %g %g\n", i+1, x[i], y[i]) ;
  }
  fclose(fp);
  //printf("End of Data Table\n");

  struct table tbl ;
  tbl.n = n ; tbl.x = x ; tbl.y = y ;
  return(tbl) ;
} //loadTable

void testTable() {
   struct table tbl = loadTable() ;
   printf("Enter x value :") ;
   float xi, yi ;
   scanf("%f", &xi);
   yi = intPolate(tbl, xi) ;
   printf("x=%f, y=%f \n", xi, yi) ;
}
