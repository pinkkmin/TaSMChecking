#include<stdio.h>
#include<stdlib.h>
#include<malloc.h>
 // #include "/root/TaSMChecking/project/runtime/tasmc.h"
int main() {

    int *p = (int*)malloc(sizeof(int)*8);
    int *q = p; // free
    /// info
    printf("p value : %zx\n", (size_t)p);
    printf("int  : %zx\n", sizeof(int));
    // initing memory
    _initTaSMC();
    
    //‭8E 6268 48A0‬
    size_t key = _f_allocatePtrKey();
    printf("pointer key: %zx\n", key);

    _f_setPointerType(&p, 0);
    _f_setPointerKey(&p, key);
    _f_addPtrToFreeTable(key);

    printf("p value: %zx\n", (size_t)p);
    _f_storeMetaData(&p, p, ( 8*sizeof(*p) + (void*)p));

    void *base = _f_loadBaseOfMetaData(&p);
    void *bound = _f_loadBoundOfMetadata(&p);

    printf("base: %zx\n", (size_t)base);
    printf("bound: %zx\n", (size_t)bound);

    size_t flag = _f_getPtrFreeFlagFromFAT(key);
    printf("flag: %zx\n", flag);
    p += 9;

    // check
    _f_checkSpatialLoadPtr(p, base, bound, sizeof(*p));





    
    free(q);
    return 0;
}
