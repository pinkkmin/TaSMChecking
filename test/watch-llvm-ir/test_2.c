#include<stdio.h>
#include<malloc.h>
int array[154] ;
int *ptr  = array + 40;

int main() {

    int a = 12;
    ptr = (int*)malloc(sizeof(int));
    *ptr = 155;
    
    free(ptr);
   *ptr = 155;
    return 0;
}

// #include<stdio.h>

// typedef struct
// {
//    int a;
//    int arr[10];
// }Node;

// Node test = {10,{15, 15, 7,7}};
// int *ptr = test.arr;

// int main() {

//    ptr += 15;

//    *ptr = 100;
//     return 0;
// }