# insert function: store base and bound to metadata

## ASM File
```
clang -Xclang -load -Xclang /root/TaSMChecking/project/libtasmc.so  test_1.c -S -o test_1.asm

```

## Obj File
```

clang -Xclang -load -Xclang /root/TaSMChecking/project/libtasmc.so  test_1.c -lrt -lm -L/root/TaSMChecking/project -ltasmc_rt -o test_1

```
