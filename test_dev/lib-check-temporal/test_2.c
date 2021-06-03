#include<stdio.h>
#include<stdlib.h>
#include<malloc.h>
//#include "/root/TaSMChecking/project/runtime/tasmc.h"

int* test() {
    size_t funcKey = 2;
    _f_allocateFunctionKey(funcKey);
    int *a;
    int b = 12;
    a  = &b;
    size_t key = _f_getFunctionKey(funcKey);
     _f_setPointerType(&a, 2);
    _f_setPointerKey(&a, key);
    return a;
}
int main() {
      
    ////////////initing////////////////
     _initTaSMC();
   /////////////////////////////
   size_t funcKey = 1;
  _f_allocateFunctionKey(funcKey);
  
  int *p  = test();
  

  _f_checkTemporalLoadPtr(p, funcKey);

    return 0;
}