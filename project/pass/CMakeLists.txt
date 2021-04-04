cmake_minimum_required(VERSION 3.16)
project(TaSMC_Pass) # tasmchecking
aux_source_directory(. DIR_SRCS) # add tasmChecking.cpp
include_directories(
    ${LLVM_PROF_INCLUDE_DIRS}
    ${PROJECT_BINARY_DIR}
    ${LLVM_INCLUDE_DIRS}
    )
link_directories(
   ${LLVM_LIBRARY_DIRS}
   )
# create libtasmc.so   
add_library(tasmc SHARED
   ${DIR_SRCS}
)
# add comliper flags 
set_target_properties(tasmc PROPERTIES
   COMPILE_FLAGS "-fno-rtti"
   )

SET(LIBRARY_OUTPUT_PATH ../../)