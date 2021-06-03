/**
 * test_demo_spatial: overflow
 * command: 
 * clang -Xclang -load -Xclang /root/TaSMChecking/project/libtasmc.so  test.c -lrt -lm -L/root/TaSMChecking/project -ltasmc_rt -o test
 * */

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
      ptr += 12;
     *ptr = 100;
     return 0;
 }
 