```
clang -Xclang -load -Xclang /root/TaSMChecking/project/libtasmc.so  test.c -lrt -lm -L/root/TaSMChecking/project -ltasmc_rt -emit-llvm-as -o test
```

```
clang -Xclang -load -Xclang /root/TaSMChecking/project/libtasmc.so  test.c -lrt -lm -L/root/TaSMChecking/project -ltasmc_rt -emit-llvm-as -o test.ll
```