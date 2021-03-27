# add-function-from-library-by-pass

## build

### create-lib

```bash
$ cd build
$ cmake ..
$ make
```

output ：`../../libtest.a`

## pass

```bash
$ cd build
$ cmake ..
$ make
```

pass output ：`../../libaddFunc.so`

## run

### opt

```bash
$  opt -load ./libaddFunc.so -addFunc test.bc 
```

### Clang

```bash
$ clang++ -Xclang -load -Xclang src/libaddFunc.so test.cpp -lrt -lm -L. -ltest -o test
```

## Others

设置链接路径不然找不到库`export LD_LIBRARY_PATH=.`

```bash
$ nm libtest.a
libTest.c.o:
                 U _GLOBAL_OFFSET_TABLE_
0000000000000017 T printHi
0000000000000000 T printLib
                 U puts
```

```bash
$ nm test
....
                 U __libc_start_main@@GLIBC_2.2.5
00000000004011c0 T main
000000000040120d T printHi
00000000004011f6 T printLib
                 U puts@@GLIBC_2.2.5
0000000000401140 t register_tm_clones

....
```

 可以看到目标文件`test`已经插入了我们的`libtest.a`中的函数`printLib`

```bash
$ ./test
hi! Info from the libTest.a
Info：run in test.cpp main.
```





:beers: Successful！！！！