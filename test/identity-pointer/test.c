#include<stdio.h>
#include<malloc.h>
int main() {
    int *a ;
    int array[100] ={10,120};
    a = array;
    int *p = (int*)malloc(sizeof(int));
    *p = 15;
    printf("Info:pointer p value: %d\n", *p);
    free(p);
    return 0;
}