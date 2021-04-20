#!/bin/bash
# run pass & create obj
clang -Xclang -load -Xclang /root/TaSMChecking/project/libtasmc.so  test_1.c -lrt -lm -L/root/TaSMChecking/project -ltasmc_rt -o test_1

# run obj
./test_1