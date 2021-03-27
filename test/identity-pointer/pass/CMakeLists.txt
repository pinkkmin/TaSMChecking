cmake_minimum_required(VERSION 3.16)
project(passDemo) # identity pointer
aux_source_directory(. DIR_SRCS) # add module.cpp
include_directories(
    ${LLVM_PROF_INCLUDE_DIRS}
    ${PROJECT_BINARY_DIR}
    ${LLVM_INCLUDE_DIRS}
    )
link_directories(
   ${LLVM_LIBRARY_DIRS}
   )
# create libaddFunc.so   
add_library(idtPtr SHARED
   ${DIR_SRCS}
)
# add comliper flags 
set_target_properties(idtPtr PROPERTIES
   COMPILE_FLAGS "-fno-rtti"
   )

SET(LIBRARY_OUTPUT_PATH ../../)