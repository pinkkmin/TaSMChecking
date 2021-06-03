# checking Spatial&Temporal for heap


```bash
 $ clang check.c -L../../project -ltasmc_rt -o check
 $ ./check
 ```

 ## check Spatial

```

p value : 13bb2a0
int  : 4
pointer key: 0
p value: 13bb2a0
base: 13bb2a0
bound: 13bb2c0
flag: 1

TaSMChecking:: In  Load Dereference Checking, base=13bb2a0, bound=13bb2c0, ptr=13bb2c4

TaSMChecking: Memory safety violation detected

Backtrace:
./check[0x4022b9]
./check[0x401c29]
./check[0x401383]
/lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0xf3)[0x7fb16b8570b3]
./check[0x40112e]


Aborted (core dumped)

```

 ## check Temporal
 ```
TaSMChecking:: temporal load check, invalid pointer key. key = 0,ptr =f182c4

TaSMChecking: Memory safety violation detected

Backtrace:
./check[0x4022ba]
./check[0x401cfe]
./check[0x401384]
/lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0xf3)[0x7f1f114100b3]
./check[0x40112e]


Aborted (core dumped)

 ```