#include<stdio.h>
#include<malloc.h>
int array[154] ;
int *ptr ;
int main() {

    int a = 12;
    ptr = (int*)malloc(sizeof(int));
    // ptr = ptr +1;
    *ptr = 12;
    return 0;
}