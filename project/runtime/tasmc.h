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
 * all const, var, func, define: start by symbol"_"
 * define : 下划线分割，全部大写
 * func：下划线加驼峰 _f_checkingTemporalAndSpatital()
 * global：_g_ 
 * const：全部大写加下划线
 * 局部： 随意 最好不要乱起
 * tips: void*表示任意传参
 *****************************************************/

#define TaSMC_MMAP_FLAGS (MAP_PRIVATE|MAP_ANONYMOUS|MAP_NORESERVE)

// invalid vlaue of pointer
#define INVALID_PTR_KEY -1
#define INVALID_PTR_BASE -1
#define INVALID_PTR_BOUND -1 

// pointer is free_able or free_unable. 
#define PTR_FREE_ABLE 1
#define PTR_FREE_UNABLE 0

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
static const size_t _FREE_ABLE_TABLE_N_KEY = ((size_t)1024 * (size_t) 8); // 2^13 ptr key
static const int _SHADOW_STACK_N_ENTRIES = ((size_t) 128 * (size_t) 32 ); // shadow stack大小，还需要乘上sizeof（void*）
//trie primary level: 2^23
static const size_t _TRIE_PRIMARY_TABLE_N_ENTRIES = ((size_t) 8 * (size_t) 1024 * (size_t) 1024);  
//trie second level: 2^22
static const size_t _TRIE_SECONDARY_TABLE_N_ENTRIES = ((size_t) 4 * (size_t) 1024 * (size_t) 1024);  

static const size_t _BITS_OF_SIZET = sizeof(size_t)*8;
static const size_t _ALIGN_BYTE_LOWER_BITS = 3; //低位3位字节对齐

// 63 ~~~ 0: highter bit no use
// 63~61: pointer type
// 60~48：pointer key
// 47~0 ：pointer address
//最高位为0 次高两位为 全1 或全0 （其他位相反）
static const size_t _BITS_62_TO_61_POS =  0x6000000000000000; // POS = 1 
static const size_t _BITS_62_TO_61_NEG =  0x1FFFFFFFFFFFFFFF; // NEG = 0

// pointer key (13bits)
static const size_t _BITS_60_TO_48_POS =  0x1FFF000000000000; 
static const size_t _BITS_60_TO_48_NEG =  0x6000FFFFFFFFFFFF;

// masking highter 16bits.
static const size_t _BITS_63_TO_48_MASK = 0xFFFFFF; 

// declare function
void _f_tasmcPrintf(const char* str, ...);
void _f_printfPointerDebug(void* ptr);
void _f_callAbort(int type);

// for trie table 
extern _tasmc_trie_entry** _trie_table;
extern _tasmc_trie_entry*  _trie_primary_level;
extern _tasmc_trie_entry* _trie_second_level;

// for free able table
extern size_t* _free_able_table;
size_t ptrKeyCounter = 0; // loop allocate：ptrKey
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
// extern void* _shadow_stack_space_begin;

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

/** remember pass &pointer to ptr
 *  like: int *p = new int;
 *        _f_setPointerType( &p, 12);
 * */
void _f_setPointerType(void* addr_of_ptr, size_t type){

    size_t value = **((size_t**)addr_of_ptr) & _BITS_62_TO_61_NEG;
    type = type <<(_BITS_OF_SIZET-3);
    value = value | type;

    *((void**)addr_of_ptr) = (void*)value;
}

/***
 * pointerKey: 60~48 bit  total:13bits = 2^13
 *      for heap: is  free_able_key
 *      for stack: is func_id_num for checking memory of temporal errors.
 *      for global: is ...
 * */
size_t _f_getPointerKey(void* ptr){
    size_t value = (size_t)ptr & _BITS_60_TO_48_POS;
    value = value >> (_BITS_OF_SIZET-16);
    return value;
}

// remember pass &pointer to ptr
void _f_setPointerKey(void* addr_of_ptr, size_t key) {

    size_t value = **((size_t**)addr_of_ptr)  & _BITS_60_TO_48_NEG;
    value = value | (key<<(_BITS_OF_SIZET-16));

    *((void**)addr_of_ptr) = (void*)value;
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
 *  ALU pointer.
 *  propagation the pointer type, key, address.
 *  the base and bound propagation by other function.
 * */
void _f_assginmentPointer(void* from, void* to) {
    size_t fromPtrKey = _f_getPointerKey(from);
    size_t fromPtrType = _f_getPoniterType(from);
}

// remember pass &pointer to parameter
// and the sizeof(*ptr)
void* _f_incPointerAddr(void* addr_of_ptr, size_t index , size_t ptr_size){

    void* ptr = *((void**)addr_of_ptr);
    void* realAddr = _f_maskingPointer(ptr);
    realAddr += index * ptr_size;
    size_t key = _f_
    *((void**)addr_of_ptr) = (void*)realAddr;
}

// remember pass &pointer to parameter
void _f_decPointerAddr(void* addr_of_ptr, size_t index, size_t ptr_size) {

    void* ptr = *((void**)addr_of_ptr);
    void* realAddr = _f_maskingPointer(ptr);
    realAddr -= index * ptr_size;
    *((void**)addr_of_ptr) = (void*)realAddr;
}

// load(store) base(bound) from metadata 
void* _f_loadBaseOfMetaData(void* ptr){
    size_t metaAddr =   _f_maskingPointer(ptr);
    _tasmc_trie_entry* entry = (_tasmc_trie_entry*)metaAddr;
    return entry->base;
}

void* _f_loadBoundOfMetadata(void* ptr) {
     size_t metaAddr =   _f_maskingPointer(ptr);
    _tasmc_trie_entry* entry = (_tasmc_trie_entry*)metaAddr;
    return entry->bound;
}

void _f_storeMetaData(void* ptr, void* base, void* bound){
    size_t ptrAddr = _f_maskingPointer(ptr);
    size_t primayIndex;
    _tasmc_trie_entry* secondLevelTrie;
    primayIndex = (ptrAddr>>25);
    secondLevelTrie = _trie_table[primayIndex];
    if(secondLevelTrie == NULL) {
        secondLevelTrie = _f_trie_allocate();
    }
} 


void _f_deallocatePointer(void* ptr) {

}


//load(store) base(bound) from shadow stack
void* _f_loadBaseOfShadowStack(int args_no){

    assert(args_no >= 0);

    size_t index = _BASE_INDEX + args_no * _METADATA_NUM_FIELDS + 2;
    size_t* base = _shadow_stack_ptr + index; 

    return base;
}

void* _f_storeBoundOfShadowStack(int args_no){

     assert(args_no >= 0);

     size_t index = _BOUND_INDEX + args_no * _METADATA_NUM_FIELDS + 2;
     size_t bound = _shadow_stack_ptr + index;

     return bound;
}

// allocate(dellocate) meataData from shadow stack
void _f_allocateShadowStackMetadata(size_t args_no){

    size_t* preStackSizePtr = _shadow_stack_ptr + 1;
    size_t preStackSize = *((size_t*)preStackSizePtr);

    _shadow_stack_ptr += (2 + preStackSize);
    *((size_t*)_shadow_stack_ptr) =  preStackSize;

     size_t* _shadow_stack_curr_ptr = _shadow_stack_ptr + 1;
     size_t size = args_no * _METADATA_NUM_FIELDS ;

     *(_shadow_stack_curr_ptr) = size;
}
void _f_deallocateShadowStackMetaData(){

    size_t* resStackPtr = _shadow_stack_ptr;
    size_t resStackSize = *((size_t*)resStackPtr);

    assert((resStackSize >=0 && resStackSize <= _SHADOW_STACK_N_ENTRIES));

    _shadow_stack_ptr -= (2 + resStackSize);
}

////
size_t _f_allocatePtrKey(){

    size_t isUse = *(_free_able_table + ptrKeyCounter);
    size_t ans = -1;
    if(isUse != 1) {
      ans = ptrKeyCounter++;
      ptrKeyCounter %= (_FREE_ABLE_TABLE_N_KEY-1);
    }else {
        for( size_t index = 0; index < _FREE_ABLE_TABLE_N_KEY; ++index) {
            if(*(_free_able_table + index) != 1) {
                ptrKeyCounter = index+1;
                return index;
            }
        }
    }
    if(ans ==-1) {
        _f_callAbort(ERROR_FREE_TABLE_USE_UP);
        return  0;
    }
    return ans;
}

// FAT: free able table
size_t _f_getPtrFreeFlagFromFAT(size_t ptrKey) {
    return *(_free_able_table + ptrKey);
}

void _f_setPtrFreeFlagToFAT(size_t ptrKey, size_t flag){
    *(_free_able_table + ptrKey) = flag;
}

/** add a pointer to the free_able_table
 *  allocate a ptr_key by pointerKeyCounter to the pointer
 *  set the flag is PTR_FREE_ABLE
 **/
void _f_addPtrToFreeTable(size_t ptrKey) {

    size_t flag = _f_getPtrFreeFlagFromFAT(ptrKey);

    if(flag == PTR_FREE_ABLE) {
        _f_tasmcPrintf("pointer key insert to free_able_talbe conflict.\n");
        _f_callAbort(ERROR_FREE_TABLE_CONFLICT);
    }

    _f_setPtrFreeFlagToFAT(ptrKey, PTR_FREE_ABLE);
}

/** remove the pointer key from free_able_table
 *  set the flag is PTR_FREE_UNABLE
 **/
void _f_removePtrFromFreeTable(void* ptr) {

    assert(ptr!=NULL);
    size_t ptrKey = _f_getPointerKey(ptr);
    _f_setPtrFreeFlagToFAT(ptrKey, PTR_FREE_UNABLE);
}

/** ask: the pointer is free able?
 *  return falg: able or unable
 **/
size_t _f_isFreeAbleOfPointer(void* ptr) {
    size_t ptr_key = _f_getPointerKey(ptr);
    size_t flag = *(_free_able_table + ptr_key);
    if(flag ==PTR_FREE_ABLE) return flag;
    return PTR_FREE_ABLE;
}

// propagation pointer metadata, key, type.
void _f_copyMetaData(void* from, void* dest){

    size_t fromAddr = _f_maskingPointer(from);
    size_t fromType = _f_getPointerKey(from);
    size_t fromKey  = _f_getPoniterType(from);


}

void _f_copyPtrInfo(void* from, void* dest) {

}
// checking temporal and spatital， dereference
void _f_checkSpatialLoadPtr(void* ptr, void* base, void* bound, size_t size){

}

void _f_checkSpatialStorePtr(void* ptr, void* base, void* bound, size_t size){

}

void _f_checkTemporalLoadPtr(void* ptr, void* base, void* bound, size_t size){

}

void _f_checkTemporalStorePtr(void* ptr, void* base, void* bound, size_t size) {

}

// this part implemented in tasmc.c
_tasmc_trie_entry* _f_trie_allocate();
_tasmc_trie_entry* _f_allocateSecondaryTrieRange();
void * _f_safe_mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset);
void* _f_malloc(size_t size);
void* _f_free(void* ptr);

#endif
