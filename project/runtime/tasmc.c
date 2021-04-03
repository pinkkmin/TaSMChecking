#include "tasmc.h"  // 我的头文件


// for trie table
_tasmc_trie_entry** _trie_table;
_tasmc_trie_entry*  _trie_primary_level;
_tasmc_trie_entry* _trie_second_level;

// for shadow stack
void* _shadow_stack_ptr = NULL;
void* _shadow_stack_curr_ptr = NULL;
void* _shadow_stack_space_begin = NULL;