#include <stdio.h>
#include "../include/avl.h"

void print_int(void* data)
{
    int value = 0;
    memcpy(&value, data, sizeof(int));
    printf("%d", value);
}

int avl_compare(const void * arg1, const void * arg2)
{
    int return_val = 0;

    if (arg1 < arg2)
    {
        return_val = -1;
    }
    else if (arg1 == arg2)
    {
        return_val = 0;
    }
    else
    {
        return_val = 1;
    }

    return return_val;
}

int main()
{
    print_f avl_printer = print_int;
    cmp_f compare = &avl_compare;
    avl_t * avl_tree = avl_create(compare);

    if (!avl_tree)
    {
        printf("Failed to create AVL tree.");
    }

    int data = 5;

    for (int idx = 0; idx < 100; idx++)
    {
        avl_insert(avl_tree, &data);
        data = data + rand() % 10;
        print_visual(avl_tree, avl_printer);
    }

    return 0;

}
