#include<stdio.h>
#include<stdlib.h>

int main() {

    int *p ;
    int array[100];

    p = array; 
    printf("%zx, %zx\n", &p, array);
    printf("%d\n", sizeof(p));
    printf("%zx, %zx\n", p, &array[100]);
}