typedef  struct 
{
    char ch;
    int val;
    int *ptr;
}struct_has_ptr;

int a = 10;
struct_has_ptr st_has_ptr = {'a', 10, &a};