int *ptr_null;
int const *ptr_const_null;
const int *ptr_to_const_null;
const int a  = 10;
const int  *ptr_const_a = &a;

const int array[100];
int const *ptr_to_const = array;

int *ptr_array[10];

void test() {
   
   *ptr_array[12] = 12;
}