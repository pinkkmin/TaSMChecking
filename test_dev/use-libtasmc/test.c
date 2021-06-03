#include<stdio.h>
#include<stdlib.h>
#include<malloc.h>
//#include "/root/TaSMChecking/project/runtime/tasmc.h"
int main() {

    int *p = (int*)malloc(sizeof(int)*8);
    int *q = p; // free

    printf("p value: %zx\n\n",(size_t)p);

    // set type
    _f_setPointerType(&p, 2);
    printf("p set ty:01 : %zx\n",(size_t)p);

    // set key
    _f_setPointerKey(&p, 255);
    printf("p set key:255: %zx\n\n",(size_t)p);

    // p += 2
    _f_incPointerAddr(&p, 2, sizeof(*p));
    printf("p value inc(index = 2): %zx\n",(size_t)p);
    int tp = _f_maskingPointer(p);
    printf("p tag : %zx\n",(size_t)p);
    printf("p real addr: %zx\n\n",(size_t)tp);

    // p -= 1
    _f_decPointerAddr(&p, 1, sizeof(*p));
    printf("p value dec(index = 1) : %zx\n",(size_t)p);
    tp = _f_maskingPointer(p);
    printf("p real addr: %zx\n\n",(size_t)tp);

    size_t type = _f_getPointerType(p), key = _f_getPointerKey(p);
    printf("p tag value = %zx  , type = %zx  , key = %zx , real adddr = %zx \n\n", 
            (size_t)p, type, key, (size_t)tp);

    free(q);
    return 0;
}
// 1 1111 1001 1101 0010 1010 0000
//   1111 1001 1101 0010 1010 0100‬