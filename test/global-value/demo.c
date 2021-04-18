#include<stdio.h>
#include<stdlib.h>
int array[100] = {121};
int main() {

   array[101] = 100;
   printf("%d\n", array[101]);
   return 0;
}