#include<stdio.h>
#include<stdlib.h>

int array[100] = {0,12,3,4};

int *arrat_int_ptr[100];

int *ptr = &array[1];
void test(char ch, int array[10],int *ptr, int arr[12]){
 //*arrat_int_ptr[11] = 12;

 size_t value_of_array = (size_t)(array);
//  printf("value_of_array : %zx\n",value_of_array);
//  printf("addr_of_array : %zx\n",(size_t)(&array));
//  printf("value_of_array[0] : %zx\n",(size_t)(array[0]));
}
int main(){

 printf("ptr : %zx, base: %zx, bound : %zx, addr_of_ptr : %zx\n",(size_t)ptr,(size_t)array,(size_t)(array+100),(size_t)(&ptr));
 return 0;
}