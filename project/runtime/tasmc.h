#ifndef __TASMC_H__
#define __TASMC_H__

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <limits.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

/*************** protocol of symbol******************
 * all const, var, func, define: 以下划线_开头
 * define : 下划线分割，全部大写
 * func：下划线加驼峰 _f_checkingTemporalAndSpatital()
 * global：_g_ 
 * const：全部大写加下划线
 * 局部： 随意 最好不要乱起
 *****************************************************/

#define _MMAP_FLAGS (MAP_PRIVATE|MAP_ANONYMOUS|MAP_NORESERVE)

typedef struct 
{
    /* data */
    void* base;
    void* bound;
    #define _METADATA_NUM_FIELDS 2
    #define _BASE_INDEX 0
    #define _BOUND_INDEX 1
} _tasmc_trie_entry;

//static const 
static const int _SHADOW_STACK_N_ENTRIES = ((size_t) 128 * (size_t) 32 );; // shadow stack大小，还需要乘上sizeof（void*）
static const int _ALIGN_BYTE_LOWER_BITS = 3; //低位3位字节对齐
//查找树一级level 2^23
static const size_t _TRIE_PRIMARY_TABLE_ENTRIES = ((size_t) 8 * (size_t) 1024 * (size_t) 1024);  
//二级查找树 第二级 2^22
static const size_t _TRIE_SECONDARY_TABLE_ENTRIES = ((size_t) 4 * (size_t) 1024 * (size_t) 1024);  


// for trie table
extern _tasmc_trie_entry** _trie_table;
extern _tasmc_trie_entry*  _trie_primary_level;
extern _tasmc_trie_entry* _trie_second_level;

// for shadow stack : 当函数参数为指针时，通过shadow stack 传递指针base和bound
/*
******************
*  ptr1_base
* ------------------ 
*  ptr1_bound
*-------------------
* ... args ....        : 假如有更多的函数参数是指针 继续添加 ptr2_base,ptr2_bound
* ------------------ 
*   ret_base           : 返回值是指针时 传递 ret_ptr的base和bound
*   ret_bound
* ------------------           
*  current_size        <---  curr_ptr  // 记录当前函数传递的函数参数空间大小 ：size = (args_ptrs_no+1) * (size)metadata * sizeof(void*)
* ------------------     
*  pre_size            <---  stack_ptr   // 保存pre_ptr  add: stack_ptr += 2  sub: stack_ptr -= 2
* ------------------                    // curr_ptr = stack_ptr + 1          
*   ...   ...        
******************** <-- shadow_stack_space_begin 
*/
extern void* _shadow_stack_ptr;
extern void* _shadow_stack_curr_ptr;
extern void* _shadow_stack_space_begin;


/**  
 * pointerType: heap(000) stack(001) global(010) others(011)
 * highter 3 bits of pointer(63~61bit)
 * */
size_t _f_getPoniterType(void* ptr){
  
}

void* _f_setPointerType(void* ptr) {

}

/***
 * pointerKey: 60~48 bit  total:13bits = 2^13
 *      for heap: is  free_able_key
 *      for stack: is func_id_num for checking memory of temporal errors.
 *      for global: is ...
 * */
size_t _f_getPointerKey(void* ptr){

}

void* _f_setPointerKey(void* ptr) {

}

/***
 * when operating pointer,such as ptr++, ptr-- ... 
 * masking ptr with highter 16 bits.
 * */
void* _f_maskingPointer(void* ptr){

}

/**
 *  assginment pointer.
 *  propagation the pointer type, key, address.
 *  the base and bound propagation by other function.
 * */
void* _f_assginmentPointer(void* ptr) {

}



//

void* _f_loadBaseOfMetaData(void* ptr){

}

void* _f_storeBoundOfMetaBound(void* ptr){

}


//

void* _f_loadBaseOfShadowStack(int args_no){

}

void* _f_storeBoundOfShadowStack(int args_no){

}



/**
 * 
 * 
 * 
 * */
_tasmc_trie_entry* _f_trieEntryAllocate(){

}



















// this part implemented in tasmc.c
void* _f_malloc(size_t size);
void* _f_free(void* ptr);
#endif