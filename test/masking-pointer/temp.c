#include<stdio.h>
#include<stdlib.h>
#include<malloc.h>

/***
 * test: void* ++
 * */
int main() {

    int* temp = malloc(1);
    size_t value = (size_t)temp;
    printf("%zu\n", value);
    free(temp);
    return 0;
} 