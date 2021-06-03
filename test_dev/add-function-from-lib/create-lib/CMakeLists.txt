cmake_minimum_required(VERSION 3.16)
project(libDemo) 
add_library(test STATIC libTest.c)
# lib output dir: add-function-from-lib
SET(LIBRARY_OUTPUT_PATH ../../)
message("Info:create static library success!!!")