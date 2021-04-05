/**
 * @author: mingfu(silence.pink) @NUAA.
 * @Date: 2021-04-01
 * Copyright (c) 2021 mingfu. All rights reserved.
 * */

#ifndef __TASMC_ERRORS_H__
#define __TASMC_ERRORS_H__

// FOR unknow
#define ERROR_POINTER_UNKNOW 0

// for pointer 
#define ERROR_POINTER_TYPE 1
#define ERROR_POINTER_KEY  2

// for memory 
#define ERROR_ALLOCATE_FAIL 10
#define ERROR_FREE_FAIL 11

#define ERROR_OF_SPATIAL 20
#define ERROR_OF_TEMPORAL 21
#define ERROR_OF_SPATIAL_LDC  22 // load dereference check
#define ERROR_OF_SPATIAL_SDC  23 // store dereference check
#define ERROR_OF_TEMPORAL_LDC 24
#define ERROR_OF_TEMPORAL_SDC 25
// for free able table 
#define ERROR_FREE_TABLE_USE_UP 100
#define ERROR_FREE_TABLE_CONFLICT 101

#endif