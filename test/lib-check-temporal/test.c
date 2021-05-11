#include<stdio.h>
#include<stdlib.h>
#include<malloc.h>
//#include "/root/TaSMChecking/project/runtime/tasmc.h"
int main() {
      
      ////////////initing////////////////
     _initTaSMC();
     /////////////////////////////
    int *p = (int*)malloc(sizeof(int)*8);
    int *q = p; // free

    // set type and set key
    size_t key = _f_allocatePtrKey();
    _f_setPointerType(&p, 1);
    _f_setPointerKey(&p, key);
    _f_addPtrToFreeTable(key);
    
    size_t flag_is_free_able = _f_isFreeAbleOfPointer(p);

    if(flag_is_free_able) {
        printf("yes, is free able.\n");
    }else {
        printf("no, can't free.\n");
    }


    _f_free(p);
    flag_is_free_able = _f_isFreeAbleOfPointer(p);
    if(flag_is_free_able) {
        printf("yes, is free able.\n");
    }else {
        printf("no, can't free.\n");
    }

    int *val = (int*)malloc(sizeof(int)*8);
    free(val);
    free(val);

    return 0;
}