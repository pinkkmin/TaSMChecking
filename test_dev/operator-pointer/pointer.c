#include<stdio.h>
#include<stdlib.h>
#include<malloc.h>
//#include "/root/TaSMChecking/project/runtime/tasmc.h"

/***    test function list
 *  void* _f_cmpPointerAddr(void* ptrLhs, void* ptrRhs, int op)
 *  void* _f_typeCasePointer(void* ptr)
 * */
int main() {

     _initTaSMC();
    int *p = (int*)malloc(sizeof(int)*8);
    int *q = p, *pr = p +1;
    printf("p addr : %0x\n", (size_t)p);

    int flag = (int)_f_cmpPointerAddr(p, pr, 11);
    if(flag ) {
        printf("pointer euqal！！！\n");
    }
    free(p);
    return 0;
}