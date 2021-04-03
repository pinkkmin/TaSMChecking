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
#include <math.h>
#include "tasmcErrors.h"
/*************** protocol of symbol******************
 * all const, var, func, define: 以下划线_开头
 * define : 下划线分割，全部大写
 * func：下划线加驼峰 _f_checkingTemporalAndSpatital()
 * global：_g_ 
 * const：全部大写加下划线
 * 局部： 随意 最好不要乱起
 *****************************************************/

#define TaSMC_MMAP_FLAGS (MAP_PRIVATE|MAP_ANONYMOUS|MAP_NORESERVE)

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
//trie primary level: 2^23
static const size_t _TRIE_PRIMARY_TABLE_N_ENTRIES = ((size_t) 8 * (size_t) 1024 * (size_t) 1024);  
//trie second level: 2^22
static const size_t _TRIE_SECONDARY_TABLE_N_ENTRIES = ((size_t) 4 * (size_t) 1024 * (size_t) 1024);  

static const size_t _BITS_OF_SIZET = sizeof(size_t)*8;
static const size_t _ALIGN_BYTE_LOWER_BITS = 3; //低位3位字节对齐

//最高位为0 次高两位为 全1 或全0 （其他位相反）
static const size_t _BITS_63_TO_61_POS =  pow(2,63) - pow(2, 61) ; // POS = 1 
static const size_t _BITS_63_TO_61_NEG =  pow(2, 62) - 1; // NEG = 0

// pointer key (13bits)
static const size_t _BITS_60_TO_48_POS =  pow(2,63) - pow(2, 61); 
static const size_t _BITS_60_TO_48_NEG =  _BITS_63_TO_61_POS + pow(2, 48) - 1;

// masking highter 16bits.
static const size_t _BITS_63_TO_48_MASK = pow(2, 48) - 1; 

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
    size_t value = (size_t)ptr;
    size_t ty = value>>(_BITS_OF_SIZET-3);
    ty = ty&7;
    if(ty > 3) _f_callAbort(ERROR_POINTER_TYPE);
    return ty;
}

void* _f_setPointerType(void* ptr, size_t type){
    size_t value = (size_t)ptr & _BITS_63_TO_61_NEG;
    type = type <<(_BITS_OF_SIZET-3);
    value = value | type;
    return (void*)value;
}

/***
 * pointerKey: 60~48 bit  total:13bits = 2^13
 *      for heap: is  free_able_key
 *      for stack: is func_id_num for checking memory of temporal errors.
 *      for global: is ...
 * */
size_t _f_getPointerKey(void* ptr){
    size_t value = (size_t)ptr & _BITS_63_TO_61_NEG;
    value = value >> (_BITS_OF_SIZET-16);
    return value;
}

void* _f_setPointerKey(void* ptr, size_t key) {
    size_t value = (size_t)ptr & _BITS_60_TO_48_NEG;
    value = value | (key<<(_BITS_OF_SIZET)-16);
    return (void*)value;
}

/***
 * when operating pointer,such as ptr++, ptr-- ... 
 * masking ptr with highter 16 bits.
 * */
void* _f_maskingPointer(void* ptr){
    size_t value = (size_t)ptr & _BITS_63_TO_48_MASK;
    return (void*)value;
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


// this part implemented in tasmc.c
_tasmc_trie_entry* _allocateSecondaryTrieRange();
void* _f_malloc(size_t size);
void* _f_free(void* ptr);
#endif

// printf error message... ...
void _f_callAbort(int type) {
    fprintf(stderr, "\nTaSMChecking: Memory safety violation detected\n\nBacktrace:\n");

    switch (type)
    {
    case ERROR_POINTER_TYPE:
        printf(stderr, "abort: pointer type error... ... \n");
        break;
    case ERROR_POINTER_KEY:
        printf(stderr, "abort: pointer key error... ... \n");
        break; 
    case ERROR_POINTER_UNKNOW:
    default:
        printf(stderr, "abort: unknow error... ... \n");
        break;
    }

    size_t size;
    void *array[100];
    size = backtrace(array, 100);
    backtrace_symbols_fd(array, size, fileno(stderr));
    fprintf(stderr, "\n\n");
    abort();
}

