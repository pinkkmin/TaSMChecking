struct array_ele
{
    char a;
    int b;
};
struct array_1
{
    struct array_ele a[100];
    int b;
};
struct array_2
{
    struct array_1 a;
    int b;
};
const struct array_ele array_struct[100] = {{'a',122}, {'b',100},{'a',122}, {'b',100},{'a',122}, {'b',100}};
const struct array_1 test[100] = {{{{'a',122}, {'b',100}},1}};
const struct array_2 ads;
void a() { 
  //  array_struct[15].a = 'A';
}
