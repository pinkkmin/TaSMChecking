typedef struct {
  char ch;
  int data;
} struct_simple; // 简单类型 结构体


typedef struct {
  int ch;
  int data[100];
} struct_has_array; // 带数组的结构体

const struct_has_array struct_3 = {'a',{1,2,3,4}};

int *p =  &struct_3.data[15];