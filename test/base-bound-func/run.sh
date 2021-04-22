#!/bin/bash
# run pass & create obj
rm -rf test_1 test_1.ll test_1.asm
clang -Xclang -load -Xclang /root/TaSMChecking/project/libtasmc.so  test_1.c -lrt -lm -L/root/TaSMChecking/project -ltasmc_rt -o test_1
#echo "clang---------------------------------------------------"
# asm
#clang -Xclang -load -Xclang /root/TaSMChecking/project/libtasmc.so  test_1.c -S -o test_1.asm

# .ll 
clang -emit-llvm -S test_1.c -o test_1.ll
echo "clang---------------------------------------------------"
# run obj
./test_1