#ifndef RTABLE_H 
#define RTABLE_H
struct table {
    int n ; //  number of entries
    float* x ;
    float* y ;
} ;

float intPolate(struct table tbl, float xi) {
int getLineCount(FILE* fp) ;
struct table loadTable () ;
void testTable() ;
#endif
