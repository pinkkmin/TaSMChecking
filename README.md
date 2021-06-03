# TaSMCheck

The Temporal and Spatial Memory Safety Checking For C at Runtime.

```bash
|- Mibench
   |- adpcm
   |- basicmath
   |- CRC32
   |- FFT
   |- qsort
   |- sha
   |- susan

|- test_dev
   |- add-function-from-lib
   |- identity-pointer
   |- maksing-pointer
   |- use-libtasmc
   |- check-spatial
   |- test-shadow-stack

|- project
   |- rumtime
   |- pass

|- test_unit
   |- Spatial
   |- Temporal
```

## Todo

- [x] writing runtime library
   - [x] for heap
   - [x] for stack
   - [x] for global
- [x] writing LLVM Pass 
- [ ] Writing note/blog
   - [x] developing pass out of source
   - [x] learning pass: create [LLVM IR]
- [x] design&review
- [x] testing
- [x] listen to **Sugar Plum Ferry**. 
- [ ] check more.....
- [ ] bugs.....

## REF

