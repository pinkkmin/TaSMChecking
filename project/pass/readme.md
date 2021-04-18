# TaSMChecking LLVM Pass
 
## Todo
- [ ] **initing function from runtime.**
- [ ] **initing [stack] Spatial&Temporal function.**
- [ ] **start to insert insert function.**



## RUN
```
clang -Xclang -load -Xclang ./libtasmc.so demo.c -emit-llvm -S -L./libtasmc_rt.a -ltasm_rt -lrt -o  demo.ll
```