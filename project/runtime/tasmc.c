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


// for trie table
_tasmc_trie_entry** _trie_table;
_tasmc_trie_entry*  _trie_primary_level;
_tasmc_trie_entry* _trie_second_level;

//free able table for heap
size_t* _free_able_table;

// for shadow stack
void* _shadow_stack_ptr = NULL;
void* _shadow_stack_curr_ptr = NULL;
// void* _shadow_stack_space_begin = NULL;

/** init memory for tasmc.
 *  primary mmap: shadow stack, free able table, trie(primary level & second level).
 * */
void _initTaSMC(){

    size_t triePrimaryLevelLength = _TRIE_PRIMARY_TABLE_N_ENTRIES * sizeof(void*);
    _trie_primary_level = mmap(0, triePrimaryLevelLength, 
					    PROT_READ| PROT_WRITE, 
					    TaSMC_MMAP_FLAGS, -1, 0);
  assert(_trie_primary_level != (void *)-1);  

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
  _shadow_stack_curr_ptr = _shadow_stack_ptr + 1 ;
  *((size_t*)_shadow_stack_curr_ptr) = 0;
}

_tasmc_trie_entry* _allocateSecondaryTrieRange(){

}

void * __f_safe_mmap(void* addr, 
                     size_t length,int prot, 
                     int flags,int fd, 
                     off_t offset){

  return mmap(addr, length, prot, flags, fd, offset);
}

void* _f_malloc(size_t size){

  return malloc(size);
}

void* _f_free(void* ptr){

  free(ptr);
}