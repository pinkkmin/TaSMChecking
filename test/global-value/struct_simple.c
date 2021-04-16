typedef struct {
  char ch;
  int data;
} struct_simple; // 简单类型 结构体

typedef struct {
  int ch;
  int data[100];
} struct_has_array; // 带数组的结构体

typedef struct {
  char *ch;
  int data;
} struct_has_ptr; // 简单类型 结构体
typedef struct {
  int a;
  struct_has_array *ptr;
} struct_has_ptr2;

const struct_simple struct_1 = {'a', 12};
const struct_has_array struct_2 = {'a', {1, 2, 3, 4}};
const struct_simple struct_1_array[15] = {{'a', 1}};

const struct_has_ptr struct_3 = {"abcdef", 12};
const struct_has_ptr2 struct_4 = {12, {'a', {1, 2, 3, 4}}};
int *p1 = &struct_1.data;
char *ch_ptr1 = &struct_1.data;

int *p2 = &struct_2.data[15];
char *ch_ptr2 = struct_2.data;

const int array[100];