typedef struct {
  char ch;
  int data;
  int a;
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
 const struct_has_array struct_2 = {11, {1, 2, 3, 4}};
struct_simple struct_1_array[15] = {{'a', 1}};

 const struct_has_ptr struct_3 = {"abcdef", 12};
 const struct_has_ptr2 struct_4 = {12, {'a', {1, 2, 3, 4}}};
int *p1 = &struct_1.data;
char *ch_ptr1 = &struct_1.data;

 int *p2 = &struct_2.data[158];
 char *ch_ptr2 = struct_2.data;

int array[100] = {0,12,3,4};
const int *ptrArray[1212];
const int ptr_to_a =1;
struct_simple array1[123];
int *arrat_int_ptr[100];
void test(){
// array[12] = 12;
// array[102] = 12;
// struct_1_array[16].data = 1;
// int array_7[121] = {12,2,3,4,212};
// array_7[124] = 21;
// array1[2310].data = 12;
// ptrArray[12] = &ptr_to_a;
 *arrat_int_ptr[11] = 12;
}
int main() {

}