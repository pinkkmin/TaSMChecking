#!/bin/bash
# run pass & create obj
rm -rf test_2 test_2.ll 
echo ""
echo "--------------------------------------------------------------------------------------------------------"
echo ""

clang -Xclang -load -Xclang /root/TaSMChecking/project/libtasmc.so  test_2.c -lrt -lm -L/root/TaSMChecking/project -ltasmc_rt -o test_2
clang test_2.c -emit-llvm -S -o test_2.ll
echo ""
echo "--------------------------------------------------------------------------------------------------------"
echo ""
# run obj
./test_2


echo ""
echo "--------------------------------------------------------------------------------------------------------"
echo ""