#  Test_demo

## Run

```Bash
$ cd spatail # or cd temporal
$ clang -Xclang -load -Xclang /root/TaSMChecking/project/libtasmc.so  test.c -lrt -lm -L/root/TaSMChecking/project -ltasmc_rt -o test
$ ./test
```

## Spatial: overflow

```bash

TaSMChecking:: In  Store Dereference Checking, ptr=7ffdc8c688e0, base=7ffdc8c688b0, bound=7ffdc8c688e0, size=4

TaSMChecking: Memory safety violation detected

Backtrace:
abort: spatial error with store ... ... 
./test[0x403080]
./test[0x4022f7]
./test[0x4014b5]
./test[0x40326b]
/lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0xf3)[0x7f3cddac20b3]
./test[0x40112e]


Aborted (core dumped)
```

## Temporal:

```bash

TaSMChecking:: temporal load check, invalid pointer key. key = 1,ptr =2001000000407090

TaSMChecking: Memory safety violation detected

Backtrace:
abort: temporal error with load... ... 
./test[0x40318c]
./test[0x4024db]
./test[0x40155a]
./test[0x403377]
/lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0xf3)[0x7f86cc1f10b3]
./test[0x40112e]


Aborted (core dumped)
```