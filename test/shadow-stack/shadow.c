#include<stdio.h>
#include<stdlib.h>
#include<malloc.h>
///#include "/root/TaSMChecking/project/runtime/tasmc.h"
/***  test function list
 * 
 *  void* _f_loadBaseOfShadowStack(int args_no)
 *  void* _f_storeBoundOfShadowStack(int args_no)
 *  void _f_allocateShadowStackMetadata(size_t args_no)
 *  void _f_deallocateShadowStackMetaData()
 * 
 * */
void test(int *p,int a){
    printf("****load base&bound****\n");
    void* addr_base =  _f_loadBaseOfShadowStack(1);
    void* addr_bound = _f_loadBoundOfShadowStack(1);
    printf("base: %0x, bound: %0x\n", (size_t)addr_base, (size_t)addr_bound);
}
int main() {

     _initTaSMC();
     int array[100] = {10};
     size_t addr_base = (size_t)array;
     size_t addr_bound = (size_t)(array+100);
     printf("base: %0x, bound: %0x\n", addr_base, addr_bound);
     int arg_ptr_no = 2;
     _f_allocateShadowStackMetadata(arg_ptr_no);
     _f_storeBaseOfShadowStack(array, 1);
     _f_storeBoundOfShadowStack(array+100, 1);
     test(array, 6);
    _f_deallocateShadowStackMetaData();
    return 0;
}
