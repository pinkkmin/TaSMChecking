#include <iostream>
//#include "/root/TaSMChecking/project/runtime/tasmc.h"
using namespace std;

void _f_incPointerAddr(void* ptr){
    *((size_t**)ptr) = (size_t*)10;
}
int main()
{
    cout<<"WORD_SIZE:"<<sizeof(size_t)<<endl;
    int *p = (int*)malloc(sizeof(int)*8);
    int *q = p;
    cout<<"addr of p: "<<(size_t)p<<endl;
    cout<<"*********************************\n";
    size_t key = 2;
    _f_incPointerAddr(&p);
    cout<<"p after set key: "<<(size_t)p<<endl;
    free(q);
    return 0;
}
