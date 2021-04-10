#include<stdio.h>
#include<stdlib.h>
#include<malloc.h>
#include "/root/TaSMChecking/project/runtime/tasmc.h"
/***  test function list
 * 
 *  void* _f_loadBaseOfShadowStack(int args_no)
 *  void* _f_storeBoundOfShadowStack(int args_no)
 *  void _f_allocateShadowStackMetadata(size_t args_no)
 *  void _f_deallocateShadowStackMetaData()
 * 
 * */
void test(int *p,int a){
    printf("********\n");
    _f_loadBaseOfShadowStack(1);

}
int main() {
     _initTaSMC();
     int array[100];
     _f_storeBoundOfShadowStack();
     _f_allocateShadowStackMetadata(1);
     int arg_ptr_no = 1;
     
     test(array, 6);
     
    _f_deallocateShadowStackMetaData();
    return 0;
}
