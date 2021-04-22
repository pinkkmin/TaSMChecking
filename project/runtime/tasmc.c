/**
 * @author: mingfu(silence.pink) @NUAA.
 * @Date: 2021-04-01
 * Copyright (c) 2021 mingfu. All rights reserved.
 * */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <sys/mman.h>
#include <execinfo.h>

#include "tasmc.h"

/********************** about pointer **************************
 *    -------addr_of_ptr-------------
 *    | 63 ~ 48 | 47  ~ 22 | 21 ~ 0 |                 
 *    -------------------------------
 *    hight 16bits + primary level(2^23) + second level(2^22)
 *    -------value_of_ptr-----------
 *    | 63 ~ 61 | 60 ~ 48 | 47 ~ 0 |
 *    -----------------------------
 *    type(3bits) + key(13bits) + real_value_of_ptr(48bits)
 * *************************************************************/


// for trie table
_tasmc_trie_entry** _trie_table;
_tasmc_trie_entry* _trie_second_level;
// function key pool
size_t* _function_key_pool;
//free able table for heap
size_t* _free_able_table;

// for shadow stack
size_t* _shadow_stack_ptr = NULL;
// void* _shadow_stack_space_begin = NULL;

void _initTaSMC_ret(){
 return ;
}
/** init memory for tasmc.
 *  primary mmap: shadow stack, free able table, trie(primary level & second level).
 * */
void _initTaSMC(){
    
    //printf(" in initing function:initTaSMC\n");
    size_t triePrimaryLevelLength = _TRIE_PRIMARY_TABLE_N_ENTRIES * sizeof(_tasmc_trie_entry*);
    _trie_table = mmap(0, triePrimaryLevelLength, 
					    PROT_READ| PROT_WRITE, 
					    TaSMC_MMAP_FLAGS, -1, 0);

  //printf("debung****************\n"); 
  //printf("_trie_table: %zx\n", (size_t)_trie_table);
  assert(_trie_table != (void *)-1);  

  int* temp = (int*)malloc(1);
  //printf("temp:%zx\n", (size_t)temp); 
  _f_allocateSecondaryTrieRange(0, (size_t)temp);
  //printf("debung****************\n"); 
  size_t freeTableLength = _FREE_ABLE_TABLE_N_KEY * sizeof(void*);
    _free_able_table = mmap(0, freeTableLength, 
					    PROT_READ| PROT_WRITE, 
					    TaSMC_MMAP_FLAGS, -1, 0);
             
  assert(_free_able_table != (void *)-1); 

  size_t shadowLength = _SHADOW_STACK_N_ENTRIES * sizeof(void*);
    _shadow_stack_ptr = mmap(0, shadowLength, 
					    PROT_READ| PROT_WRITE, 
					    TaSMC_MMAP_FLAGS, -1, 0);
  assert(_shadow_stack_ptr != (void *)-1); 
  
  *((size_t*)_shadow_stack_ptr) = 0;

  size_t* _shadow_stack_curr_ptr = _shadow_stack_ptr + 1 ;
  *((size_t*)_shadow_stack_curr_ptr) = 0;

  size_t freeMapLength = _FREE_ABLE_TABLE_N_KEY * sizeof(size_t);
  _free_able_table = mmap(0, freeMapLength, 
                                          PROT_READ| PROT_WRITE, 
                                          TaSMC_MMAP_FLAGS, -1, 0);
    assert(_free_able_table != (void*) -1);

   size_t functionKeyPoolLen  = _FUNCTIONKEY_POOL_N_ITEMS * sizeof(size_t);
  _function_key_pool = mmap(0, functionKeyPoolLen, 
                                          PROT_READ| PROT_WRITE, 
                                          TaSMC_MMAP_FLAGS, -1, 0);       
    
  
   *(_function_key_pool) = 0; 
}

_tasmc_trie_entry* _f_trie_allocate(){
  _tasmc_trie_entry* secondLevel = NULL;
  size_t length = (_TRIE_SECONDARY_TABLE_N_ENTRIES) * sizeof(_tasmc_trie_entry);
  secondLevel = _f_safe_mmap(0, length, PROT_READ| PROT_WRITE, 
					      TaSMC_MMAP_FLAGS, -1, 0);

  assert(secondLevel != (void*)-1); 
  
  return (_tasmc_trie_entry*)secondLevel;
}

void* _f_safe_mmap(void* addr, 
                     size_t length,int prot, 
                     int flags,int fd, 
                     off_t offset){

  return mmap(addr, length, prot, flags, fd, offset);
}

void* _f_malloc(size_t size){

  return malloc(size);
}

void _f_free(void* ptr){

  void* realPtrAddr = _f_maskingPointer(ptr);
  free(realPtrAddr);
}


// printf error message... ...
void _f_callAbort(int type) {
    fprintf(stderr, "\nTaSMChecking: Memory safety violation detected\n\nBacktrace:\n");

    switch (type)
    {
    case ERROR_POINTER_TYPE:
          (stderr, "abort: pointer type error... ... \n");
        break;
    case ERROR_POINTER_KEY:
        fprintf(stderr, "abort: pointer key error... ... \n");
        break; 
    case ERROR_FREE_TABLE_USE_UP:
        fprintf(stderr, "abort: free able table use up... ... \n");
        break;
    case ERROR_FREE_TABLE_CONFLICT:
        fprintf(stderr, "abort: free able table insert key conflict... ... \n");
        break;
        
    case ERROR_OF_SPATIAL:
        fprintf(stderr, "abort: spatial error with check memory... ... \n");
        break;
    case ERROR_OF_TEMPORAL:
        fprintf(stderr, "abort: temporal error with check memory... ... \n");
        break;
    case ERROR_VIRTUAL_ADDR:
        fprintf(stderr, "abort: virtual address error... ... \n");
        break;
    case ERROR_FUNCTION_POOL_OVERFLOW:
        fprintf(stderr, "abort: tasmc initing abort... ... \n");
        fprintf(stderr, "abort: tasmc function pools overflow... ... \n");
        break;
    case ERROR_FUNCTION_CALLING:
        fprintf(stderr, "abort: tasmc function called error... ... \n");
        break;
    case ERROR_POINTER_UNKNOW:
    default:
        fprintf(stderr, "abort: unknow error... ... \n");
        break;
    }

    size_t size;
    void *array[100];
    size = backtrace(array, 100);
    backtrace_symbols_fd(array, size, fileno(stderr));
    fprintf(stderr, "\n\n");
    abort();
}

void _f_printfPointerDebug(void* ptr) {

    size_t value = (size_t)ptr;
    size_t ptrType = _f_getPointerType(ptr);
    size_t ptrKey = _f_getPointerKey(ptr);
    size_t ptrAddr = (size_t)_f_maskingPointer(ptr);

    printf("ptr info: value = 0x%zx,  ptrType = 0x%zx, ptrKey = 0x%zx, ptrAddr = 0x%zx.\n", value, ptrType, ptrKey, ptrAddr);
    printf("tips: ~ ( ty : 000:heap, 001:stack, 010:global, 011:others ) ! ! !\n\n");
}

void _f_debugPrinfInfo(){
  printf("[TASMC: debug output....... \n");
}

void _f_tasmcPrintf(const char* str, ...)
{
  va_list args;
  
  va_start(args, str);
  vfprintf(stderr, str, args);
  va_end(args);
} 

// pseudo main from TaSMCPass.
// called by rumtime library main() function
extern int _f_pseudoMain(int argc, char **argv);

int main(int argc, char **argv){

  #if __WORDSIZE == 32
  exit(1);
  #endif
  int retValue;
  char** new_argv = argv;
  
  printf("debug: running in runtime library main()\n");
  retValue = _f_pseudoMain(argc, new_argv);
  return retValue;
}