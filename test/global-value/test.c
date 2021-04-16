#include<stdio.h>
#include<stdlib.h>

typedef struct node_1
{
    char a;
    int b;
} node_1;

typedef struct node_2
{
    struct node_1 a[100];
    int b;
} node_2;

typedef struct node_3
{
    struct node_2 a;
    int b;
} node_3;

typedef struct node_4
{
    struct node_3 *a;
    int b;
} node_4;

const struct node_1 array_struct[100] = {{'a',122}, {'b',100},{'a',122}, {'b',100},{'a',122}, {'b',100}};
const struct node_2 test[100] = {{{{'a',122}, {'b',100}},1}};
const struct node_3 ads = {};
const struct node_1 asasa[10] = {};
struct node_3 ad12s = {};
struct node_4 array3 = {};
const char *p = "dsdsdsdsdsdsd";
const int aq = 100;
const int bq = 100;
const int array1[100] = {100,199};
int *pa  = &array_struct[85];
int main() { 
  //  array_struct[15].a = 'A';
    size_t add_pa = (size_t)pa;

    printf("%zu",add_pa);
  return 0;
}
