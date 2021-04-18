```
clang -Xclang -load -Xclang /root/TaSMChecking/project/libtasmc.so  struct_simple.c -lrt -lm -L/root/TaSMChecking/project -ltasmc_rt -emit-llvm-as -o out.ll

```

```

clang -Xclang -load -Xclang ./libgv.so  pointer.c  -emit-llvm -S -o pointer.ll

```