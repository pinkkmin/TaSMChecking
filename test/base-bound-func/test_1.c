#include<stdio.h>
#include<stdlib.h>

int array[100] = {0,12,3,4};

int *arrat_int_ptr[100];

int *ptr = &array[1];
void test(){
 *arrat_int_ptr[11] = 12;
}
int main(){
 printf("hello main()\n");
 return 0;
}