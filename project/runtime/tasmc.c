#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <sys/mman.h>
#include <execinfo.h>
#include "tasmc.h"  // 我的头文件


// for trie table
_tasmc_trie_entry** _trie_table;
_tasmc_trie_entry*  _trie_primary_level;
_tasmc_trie_entry* _trie_second_level;

// for shadow stack
void* _shadow_stack_ptr = NULL;
void* _shadow_stack_curr_ptr = NULL;
void* _shadow_stack_space_begin = NULL;


void _initTaSMC(){

    size_t triePrimaryLevelLength = _TRIE_PRIMARY_TABLE_N_ENTRIES * sizeof(void*);
    _trie_primary_level = mmap(0, triePrimaryLevelLength, 
					    PROT_READ| PROT_WRITE, 
					    TaSMC_MMAP_FLAGS, -1, 0);
  assert(_trie_primary_level != (void *)-1);  
  
}