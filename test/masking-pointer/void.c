#include<stdio.h>
#include<stdlib.h>
#include<malloc.h>

/***
 * test: void* ++
 * */
int main() {
    int *p = (int*)malloc(sizeof(int)*8); 
    int  *q = p;
    void* v_ptr = q;
    printf("ptr value:%d  , %d\n",q,++q);
    v_ptr += sizeof(*q);
    q = v_ptr;
    int q_vlaue = (int)q;
    printf("ptr after ++:%d\n",q_vlaue);
    size_t add_of_p = &p;
    printf("&p ++:%x\n",add_of_p);
    free(p);
    return 0;
}