#include<stdlib.h>
 typedef struct sub_object
 {
     int array[10];
     char ch;
     int val;
 }Node;
  
     
int main() {
   
Node test;
     int *ptr = test.array;
      ptr += 10;
     *ptr = 100;
     return 0;
 }
 