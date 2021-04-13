#include<stdio.h>
#include<stdlib.h>
#include<malloc.h>
int array[100] = {0};
int *p = NULL;
int a;
int aa;
int main(int argc, char** argv){

   printf("in original main()\n");
   int k = *array;
   printf("%d",k);
   printf("**********************************\n");
   return 0;
}