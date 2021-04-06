cmake_minimum_required(VERSION 3.16)
project(runtimeLib) 
include_directories(.)
aux_source_directory(. DIR_SRCS)
add_library(tasmc_rt STATIC ${DIR_SRCS})
# lib output dir:project
set_target_properties(tasmc_rt PROPERTIES
   COMPILE_FLAGS "-Wall -pedantic"
   )
SET(LIBRARY_OUTPUT_PATH ../../)
message("Info:create static library success!!!")