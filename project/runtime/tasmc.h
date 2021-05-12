/**
 * @author: mingfu(silence.pink) @NUAA.
 * @Date: 2021-04-01
 * Copyright (c) 2021 mingfu. All rights reserved.
 * */

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

/// DEBUG FOR TaSMC
#define TaSMC_DEBUG_FLAG 0

#define TaSMC_BOOL_TRUE 1
#define TaSMC_BOOL_FALSE 0

#define TaSMC_CMP_EQUAL 0
#define TaSMC_CMP_LESS 10  // 小于
#define TaSMC_CMP_MORE 11  // 大于
#define TaSMC_CMP_LESS_EQUAL 20  // 小于等于
#define TaSMC_CMP_MORE_EQUAL 21  // 大于等于  

#define TaSMC_SHIFT_LEFT 0  // 左移
#define TaSMC_SHIFT_RIGHT 1  // 右移 
#define TaSMC_SHIFT_LOGIC_RIGHT 10  // 逻辑右移 

#define TaSMC_MMAP_FLAGS (MAP_PRIVATE|MAP_ANONYMOUS|MAP_NORESERVE)

// invalid vlaue of pointer
#define INVALID_PTR_KEY -1
#define INVALID_PTR_BASE -1
#define INVALID_PTR_BOUND -1 

// pointer is free_able or free_unable. 
#define PTR_FREE_ABLE 1
#define PTR_FREE_UNABLE 0

// type of memory
// heap(1) stack(010) global(011) others(000)
#define TYPE_HEAP 1
#define TYPE_STACK 2
#define TYPE_GLOBAL 3
#define TYPE_OTHERS 0

// check pointer key with function key . in stack.
#define PTR_KEY_OK 1
#define PTR_KEY_FAIL 0

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

// functionKey Pool
static const size_t _FUNCTIONKEY_POOL_N_ITEMS =  ((size_t)1024 * (size_t) 128);

// 63 ~~~ 0: highter bit no use
// 63~61: pointer type
// 60~48：pointer key
// 47~0 ：pointer value (point to obejct)
//最高位为0 次高两位为 全1 或全0 （其他位相反）
static const size_t _BITS_62_TO_61_POS =  0x6000000000000000; // POS = 1 
static const size_t _BITS_62_TO_61_NEG =  0x1FFFFFFFFFFFFFFF; // NEG = 0

// pointer key (13bits)
static const size_t _BITS_60_TO_48_POS =  0x1FFF000000000000; 
static const size_t _BITS_60_TO_48_NEG =  0x6000FFFFFFFFFFFF;

// masking highter 16bits.
static const size_t _BITS_63_TO_48_MASK = 0xFFFFFFFFFFFF; 

// declare function
void _f_tasmcPrintf(const char* str, ...);
void _f_printfPointerDebug(void* ptr);
void _f_callAbort(int type);
void* _f_maskingPointer(void* ptr);
void _f_printfPtrBaseBound(void* , void* , void*);
// for trie table 
extern _tasmc_trie_entry** _trie_table;
extern _tasmc_trie_entry* _trie_second_level;
extern size_t* _function_key_pool;
// for free able table
extern size_t* _free_able_table;
size_t ptrKeyCounter = 1; // loop allocate：ptrKey
size_t functionKey = 1; // 
// for shadow stack : 当函数参数为指针时，通过shadow stack 传递指针base和bound
/*
******************
*  ptr1_base
* ------------------ 
*  ptr1_bound          : arg_no += 1
*-------------------
* ... args ....        : 假如有更多的函数参数是指针 继续添加 ptr2_base,ptr2_bound
* ------------------   
*   ret_base           : 返回值是指针时 传递 ret_ptr的base和bound
*   ret_bound          : arg_no always 0 arg_no = arg_is_pointer_count
* ------------------           
*  current_size        <---  curr_ptr  // 记录当前函数传递的函数参数空间大小 ：size = (args_ptrs_no+1) * (size)metadata * sizeof(void*)
* ------------------     
*  pre_size            <---  stack_ptr   // 保存pre_ptr  add: stack_ptr += 2  sub: stack_ptr -= 2
* ------------------                    // curr_ptr = stack_ptr + 1          
*   ...   ...        
******************** <-- shadow_stack_space_begin 
*/
extern size_t* _shadow_stack_ptr;
// extern void* _shadow_stack_space_begin;

// ***************** declaration function *************************
 void _f_callAbort(int type) ;

// this part implemented in tasmc.c
_tasmc_trie_entry* _f_trie_allocate();
void* _f_safe_mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset);
void* _f_malloc(size_t size);
void _f_free(void* ptr);

extern  void _initTaSMC_ret();
// 你也要初始化？
extern void _initTaSMC();
//好吧 全局初始化就是 初始化softboundcets 和 stub 
void _tasmc_global_init()
{
  _initTaSMC();
  _initTaSMC_ret();
}

/**  
 * pointerType: heap(001) stack(010) global(011) others(000)
 * highter 3 bits of pointer(63~61bit)
 * */
size_t _f_getPointerType(void* ptr){
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

    //printf("addr_of_ptr: %zx\n",(size_t)addr_of_ptr);
     void* real_add_of_ptr = _f_maskingPointer(addr_of_ptr);
    size_t value = *((size_t*)real_add_of_ptr) & _BITS_62_TO_61_NEG;
   // printf("addr_of_ptr: %zx\n",value);
    type = type <<(_BITS_OF_SIZET-3);
    value = value | type;

   // printf("addr_of_ptr: %zx\n",value);

    *((void**)real_add_of_ptr) = (void*)value;
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

    // printf("addr_of_ptr: %zx\n",(size_t)addr_of_ptr);
    void* real_add_of_ptr = _f_maskingPointer(addr_of_ptr);
    size_t value =  *((size_t*)real_add_of_ptr)  & _BITS_60_TO_48_NEG;
   // printf("value: %zx\n",value);
    value = value | (key<<(_BITS_OF_SIZET-16));

    *((void**)real_add_of_ptr) = (void*)value;
}

/***
 * when operating pointer,such as ptr++, ptr-- ... 
 * masking ptr with highter 16 bits.
 * */
void* _f_maskingPointer(void* ptr){
    size_t value = (size_t)ptr & _BITS_63_TO_48_MASK;
    return (void*)value;
}

// remember pass &pointer to parameter
// and the sizeof(*ptr)
void _f_incPointerAddr(void* addr_of_ptr, size_t index , size_t ptr_size){

    void* real_add_of_ptr = _f_maskingPointer(addr_of_ptr);
    void* ptr = *((void**)real_add_of_ptr);
    void* realAddr = _f_maskingPointer(ptr);
    realAddr = realAddr + (index * ptr_size);
    size_t value = (size_t)realAddr;
    if(value > 0xFFFFFFFFFFFFFFFF || value < 0) {
        _f_callAbort(ERROR_VIRTUAL_ADDR);
    }

    *((void**)real_add_of_ptr) = *((void**)real_add_of_ptr) + (index * ptr_size);
}

// remember pass &pointer to parameter
void _f_decPointerAddr(void* addr_of_ptr, size_t index, size_t ptr_size) {

    void* real_add_of_ptr = _f_maskingPointer(addr_of_ptr);
    void* ptr = *((void**)real_add_of_ptr);
    void* realAddr = _f_maskingPointer(ptr);
    realAddr = realAddr-(index * ptr_size);
    size_t value = (size_t)realAddr;
    if(value > 0xFFFFFFFFFFFFFFFF || value < 0) {
        _f_callAbort(ERROR_VIRTUAL_ADDR);
    }

    *((void**)real_add_of_ptr) = *((void**)real_add_of_ptr) - (index * ptr_size);
}

/** compare the pointer address
 * TaSMC_CMP_EQUAL 0
 * TaSMC_CMP_LESS 10  // 小于
 * TaSMC_CMP_MORE 11  // 大于
 * TaSMC_CMP_LESS_EQUAL 20  // 小于等于
 * TaSMC_CMP_MORE_EQUAL 21  // 大于等于  
 * */
void* _f_cmpPointerAddr(void* ptrLhs, void* ptrRhs, int op){
    
    void* res = 0;
    void *basePtrLhs = _f_maskingPointer(ptrLhs), *basePtrRhs = _f_maskingPointer(ptrRhs);

    switch (op)
    {
        case TaSMC_CMP_EQUAL:
        return (void*)(basePtrLhs == basePtrRhs);

        case TaSMC_CMP_LESS:
            return (void*)(basePtrLhs < basePtrRhs);

        case TaSMC_CMP_LESS_EQUAL:
            return (void*)(basePtrLhs <= basePtrRhs);
        case TaSMC_CMP_MORE:
            return (void*)(basePtrLhs > basePtrRhs);
        case TaSMC_CMP_MORE_EQUAL:    
            return (void*)(basePtrLhs <= basePtrRhs);
        default:
            break;
    }

    return res;
}

// bitwase
// void* _f_shiftPointerAddr(void* ptr, size_t op, size_t step){

//     void* base_ptr = 0;
//     switch (op)
//     {
//         case  TaSMC_SHIFT_LEFT:
//             return base_ptr<<step;
        
//         default:
//             break;
//     }
//     return base_ptr;
// }

// type case
void* _f_typeCasePointer(void* ptr) {
    return _f_maskingPointer(ptr);
}


/*********************************************************************************************************/
// function key for temporal in stack.

size_t _f_allocateFunctionKey(size_t functionId) {
     *(_function_key_pool + functionId) = functionKey++;
     if(functionKey == 8192) functionKey = 1;

     size_t key = *(_function_key_pool + functionId);
     return key;
}

void _f_deallocaFunctionKey(size_t functionId) {
    size_t functionKey =  *(_function_key_pool + functionId);
    if(functionKey <= 0) _f_callAbort(ERROR_FUNCTION_CALLING);

    *(_function_key_pool + functionId) -= 0;
}

void _f_initFunctionKeyPool(size_t functionNums){
    assert((_function_key_pool != (void*) -1) && "functionKey pool is NULL, dosen't init... ... ?");
    if(functionNums >= _FUNCTIONKEY_POOL_N_ITEMS) {
        _f_callAbort(ERROR_FUNCTION_POOL_OVERFLOW);         // call abort;
    }

    for(size_t id = 0; id < functionNums; id++) {
        *(_function_key_pool + id) = 0;
    }

}

size_t _f_getFunctionKey(size_t functionId) {

    size_t functionKey =  *(_function_key_pool + functionId);

    if(functionKey <= 0) _f_callAbort(ERROR_FUNCTION_CALLING);
    return functionKey;
}

size_t _f_checkPtrKeyWithFuncKey(void* ptr, size_t function_id){

    size_t funcKey = _f_getFunctionKey(function_id);
    size_t ptrKey = _f_getPointerKey(ptr);

    if(funcKey ==  ptrKey) return PTR_KEY_OK;
    
    return PTR_KEY_FAIL;
}
/*******************************************************************************************************/

// load(store) base(bound) from metadata 
void* _f_loadBaseOfMetaData(void* addr_of_ptr){

    void* real_add_of_ptr = _f_maskingPointer(addr_of_ptr);
    size_t addr = (size_t)_f_maskingPointer(real_add_of_ptr);
    size_t primayIndex, secondIndex;
    _tasmc_trie_entry* secondLevelTrie;
    primayIndex = (addr>>25);
    secondIndex = (addr>>3) &0x3FFFFF;
    secondLevelTrie = _trie_table[primayIndex];
    _tasmc_trie_entry* entry = &secondLevelTrie[secondIndex];

    return entry->base;
}   

void* _f_loadBoundOfMetadata(void* addr_of_ptr) {
    
    void* real_add_of_ptr = _f_maskingPointer(addr_of_ptr);
    size_t addr = (size_t)_f_maskingPointer(real_add_of_ptr);
    size_t primayIndex, secondIndex;
    _tasmc_trie_entry* secondLevelTrie;
    primayIndex = (addr>>25);
    secondIndex = (addr>>3) &0x3FFFFF;
    secondLevelTrie = _trie_table[primayIndex];
    _tasmc_trie_entry* entry = &secondLevelTrie[secondIndex];
   // _tasmc_trie_entry* entry = (_tasmc_trie_entry*)real_add_of_ptr;

    return entry->bound;
}

void _f_loadMetaData(void* addr_of_ptr, void** addr_of_base, void** addr_of_bound){

    void* real_add_of_ptr = _f_maskingPointer(addr_of_ptr);
    size_t addr = (size_t)_f_maskingPointer(real_add_of_ptr);

    size_t primayIndex, secondIndex;
    _tasmc_trie_entry* secondLevelTrie;
    primayIndex = (addr>>25);
    secondIndex = (addr>>3) &0x3FFFFF;
    secondLevelTrie = _trie_table[primayIndex];
    _tasmc_trie_entry* entry = &secondLevelTrie[secondIndex];

    *((void**) addr_of_base) = entry->base;
  *((void**) addr_of_bound) = entry->bound;

}
void _f_storeMetaData(void* addr_of_ptr, void* base, void* bound){  
    
    void* real_add_of_ptr = _f_maskingPointer(addr_of_ptr);
    size_t addr = (size_t)_f_maskingPointer(real_add_of_ptr);
    size_t primayIndex, secondIndex;
    _tasmc_trie_entry* secondLevelTrie;
    primayIndex = (addr>>25);
    secondIndex = (addr>>3) &0x3FFFFF;
    secondLevelTrie = _trie_table[primayIndex];
    if(secondLevelTrie == NULL) {
        secondLevelTrie = _f_trie_allocate();
        _trie_table[primayIndex] = secondLevelTrie;
    }

    _tasmc_trie_entry* entry = &secondLevelTrie[secondIndex];
    
    entry->base = base;
    entry->bound = bound;
    // debug output info:
    // _f_printfPtrBaseBound(addr_of_ptr, base, bound);

    // printf("*******************************************\n");
    // printf("bound: %zx\n", (size_t)bound);
    // printf("*******************************************\n");

} 

// PS: not add to pass
void _f_deallocatePointer(void* ptr) {

}


//load(store) base(bound) from shadow stack
void* _f_loadBaseOfShadowStack(int args_no){

    assert(args_no >= 0);

    size_t index = _BASE_INDEX + args_no * _METADATA_NUM_FIELDS + 2;
    size_t* base_ptr = _shadow_stack_ptr + index; 
    void* base = *(void**)base_ptr;
   
    return base;
}

void* _f_loadBoundOfShadowStack(int args_no){

   assert(args_no >= 0);

    size_t index = _BOUND_INDEX + args_no * _METADATA_NUM_FIELDS + 2;
    size_t* bound_ptr = _shadow_stack_ptr + index; 
    void* bound = *(void**)bound_ptr;    
   
    return bound;
}

void _f_storeBaseOfShadowStack(void* base, int args_no){

     assert(args_no >= 0);

     size_t index = _BASE_INDEX + args_no * _METADATA_NUM_FIELDS + 2;
     void** base_ptr = (void**)(_shadow_stack_ptr + index);
     *(base_ptr) = base;
}

void _f_storeBoundOfShadowStack(void* bound, int args_no){

     assert(args_no >= 0);

     size_t index = _BOUND_INDEX + args_no * _METADATA_NUM_FIELDS + 2;
     void** bound_ptr = (void**)(_shadow_stack_ptr + index);
     *(bound_ptr) = bound;
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
    //printf("resStackSize: %zu\n", resStackSize);
    assert((resStackSize >=0 && resStackSize <= _SHADOW_STACK_N_ENTRIES));

    _shadow_stack_ptr -= (2 + resStackSize);
}

////
size_t _f_allocatePtrKey(){

    size_t isUse = *(_free_able_table + ptrKeyCounter);
    size_t ans = -1;
    if(isUse != 1) {
      ans = ptrKeyCounter++;
      if(ptrKeyCounter >= _FREE_ABLE_TABLE_N_KEY)
      ptrKeyCounter %= 1;
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

    assert(ptr != NULL);
    size_t ptrKey = _f_getPointerKey(ptr);
    _f_setPtrFreeFlagToFAT(ptrKey, PTR_FREE_UNABLE);
}

/** ask: the pointer is free able?
 *  return falg: able or unable
 **/
size_t _f_isFreeAbleOfPointer(void* ptr) {
    size_t ptr_key = _f_getPointerKey(ptr);
    size_t flag = *(_free_able_table + ptr_key);
    if(flag == PTR_FREE_ABLE) return flag;
    return PTR_FREE_UNABLE;
}

// propagation pointer metadata, key, type.
void _f_copyMetaData(void* addr_of_from, void* addr_of_dest){
    size_t from = (size_t)addr_of_from;
    size_t dest = (size_t)addr_of_dest;
    // PS: 处理 字节不对齐情况
}

// checking temporal and spatital， dereference
void _f_checkSpatialLoadPtr(void* ptr, void* base, void* bound, size_t size){
   
    void* addr = _f_maskingPointer(ptr);
    if ((addr < base) || ((void*)(addr + size) > bound)) {
        _f_tasmcPrintf("\nTaSMChecking:: In  Load Dereference Checking, ptr=%zx, base=%zx, bound=%zx\n",
        ptr,base, bound);  
                   _f_callAbort(ERROR_OF_SPATIAL_LDC);
    }
}

void _f_checkSpatialStorePtr(void* ptr, void* base, void* bound, size_t size){
   
    void* addr = _f_maskingPointer(ptr);    
    if ((addr < base) || ((void*)(addr + size) > bound)) {
        _f_tasmcPrintf("\nTaSMChecking:: In  Store Dereference Checking, ptr=%zx, base=%zx, bound=%zx\n",
        ptr, base, bound);  
        _f_callAbort(ERROR_OF_SPATIAL_SDC);
    }

}

void _f_checkDereferencePtr(void* ptr,void* base, void* bound) {

  if ((base != bound) && (ptr != base)) {
      _f_tasmcPrintf("In Call Dereference Check, base=%zx, bound=%zx, ptr=%zx \n", (size_t)base, (size_t)bound, (size_t)ptr);
    _f_callAbort(ERROR_OF_DEREFERENCE);
  } 
}

void _f_checkTemporalLoadPtr(void* ptr, size_t functionId){

   size_t key = _f_getPointerKey(ptr);
   size_t type = _f_getPointerType(ptr);

   if(type == TYPE_HEAP) {
       size_t flag =  _f_isFreeAbleOfPointer(ptr);
       //printf("flag: %zx\n", flag);
       if(flag != PTR_FREE_ABLE){ 
           _f_tasmcPrintf("\n TaSMChecking:: temporal load check, invalid pointer key. key = %zx,ptr =%zx\n", key, ptr);
           _f_callAbort(ERROR_OF_TEMPORAL_LDC);
       }  
    }
    else {  // stack,global ...
        size_t status = _f_checkPtrKeyWithFuncKey(ptr, functionId);
        if(status != PTR_KEY_OK) {
            size_t functionKey = _f_getFunctionKey(functionId);
           _f_tasmcPrintf("\n TaSMChecking:: temporal load check, invalid pointer key. key = %zx,functionKey=%zx,ptr =%zx\n", 
                key,functionKey, ptr);
           _f_callAbort(ERROR_OF_TEMPORAL_LRS);
        }
    }
}

void _f_checkTemporalStorePtr(void* ptr, size_t functionId) {

    size_t key = _f_getPointerKey(ptr);
    size_t type = _f_getPointerType(ptr);

    if(type == TYPE_HEAP) {
       size_t flag =  _f_isFreeAbleOfPointer(ptr);

       if(flag != PTR_FREE_ABLE){ 
           _f_tasmcPrintf("\n TaSMChecking:: temporal load check, invalid pointer key. key = %zx,ptr =%zx\n", key, ptr);
           _f_callAbort(ERROR_OF_TEMPORAL_SDC);
       }  
    }
    else {  // stack,global ...
        size_t status = _f_checkPtrKeyWithFuncKey(ptr, functionId);
        if(status != PTR_KEY_OK) {
            size_t functionKey = _f_getFunctionKey(functionId);
           _f_tasmcPrintf("\n TaSMChecking:: temporal load check, invalid pointer key. key = %zx,functionKey=%zx,ptr =%zx\n", 
                key,functionKey, ptr);
           _f_callAbort(ERROR_OF_TEMPORAL_SRS);
        }
    }
}

// allocate second level trie range
void _f_allocateSecondaryTrieRange(void* start, size_t size){

  size_t startAddr = (size_t)start;
  size_t ednAddr = (size_t)((void*)(start + size));
  size_t primaryStart = startAddr >> 25;
  size_t primaryEnd = ednAddr >> 25;

//   printf("start:%zx,  end: %zx\n", (size_t)startAddr, (size_t)ednAddr); 
//   printf("primaryStart:%zx,  primaryEnd: %zx\n", (size_t)primaryStart, (size_t)primaryEnd); 

  while (primaryStart <= primaryEnd)
  {
      _tasmc_trie_entry* secondlevelTrie = _trie_table[primaryStart];
      if(secondlevelTrie == NULL) {
          secondlevelTrie = _f_trie_allocate();
          _trie_table[primaryStart] = secondlevelTrie;
      }
      primaryStart ++;
  }
  
}

// add debug printf ptr info
void _f_printfPtrBaseBound(void* addr_of_ptr, void* base, void* bound){
    void *ptr = *((void**)addr_of_ptr);
    printf("[ tasmc debug ] ");
    printf(" ptr: %zx, ", (size_t)ptr);
    printf(" *ptr: %d, ", (*(int*)ptr));
    printf(" base: %zx, ", (size_t)base);
    printf(" bound : %zx, ", (size_t)bound);
    printf(" addr_of_ptr : %zx \n", (size_t)addr_of_ptr);
}
#endif
