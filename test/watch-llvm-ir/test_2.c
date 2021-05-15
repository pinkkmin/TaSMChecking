#include<stdio.h>
#include<malloc.h>
int array[154] ;
int *ptr  = array + 400;
void test(int *p, int a, int *c){
    int aa = 123;
}
int main() {

    int a = 12;
    ptr = (int*)malloc(sizeof(int));
    *ptr = 155;
    //ptr += 15;
    ptr = &a; 
    // test(ptr, 12, ptr);
    // test(ptr, 12, ptr);

    free(ptr);
   // free(ptr);
    return 0;
}