# test : operating pointer

 - masking pointer address
 - pointer inc
 - pointer dec
 - set/get type
 - set/get key



```
p value: 12b52a0

p set ty:01 : 40000000012b52a0
p set key:255: 40ff0000012b52a0

p value inc(index = 2): 40ff0000012b52a8
p tag : 40ff0000012b52a8
p real addr: 12b52a8

p value dec(index = 1) : 40ff0000012b52a4
p real addr: 12b52a4

p tag value = 40ff0000012b52a4  , type = 2  , key = ff , real adddr = 12b52a4 

```