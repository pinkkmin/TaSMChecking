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
 
 printf("ptr : %zx, base: %zx, bound : %zx, addr_of_ptr : %zx\n",(size_t)ptr,(size_t)array,(size_t)(array+100),(size_t)(&ptr));

 return 0;
}