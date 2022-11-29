#include <stdio.h>
#include <stdlib.h>

#include "../include/avl.h"

typedef struct node_t
{
    struct node_t * parent;
    struct node_t * left;
    struct node_t * right;
    void *          data;
    int             height;
} node_t;

typedef struct avl_t
{
    node_t * root;
    cmp_f    cmp;
}avl_t;

typedef struct trunk
{
    struct trunk * prev;
    const char *   str;
}trunk_t;

static node_t * node_insert(node_t * root, void * data, cmp_f cmp);
static node_t * rotate_left(node_t ** root, node_t * new_node, cmp_f cmp);
static node_t * rotate_right(node_t ** root, node_t * new_node, cmp_f cmp);
static void     print_recursive(node_t *       root,
                                struct trunk * prev,
                                int            is_left,
                                print_f        printer);
static void     print_trunks(struct trunk * p);
static void     recurse_delete(node_t * node, del_f del);

avl_t *
avl_create(cmp_f f_cmp)
{
    if (!f_cmp)
    {
        return NULL;
    }
    avl_t * tree = calloc(1, sizeof(*tree));
    if (tree)
    {
        tree->cmp = f_cmp;
    }
    return tree;
}

static node_t *
node_insert(node_t * root, void * data, cmp_f cmp)
{
    int result = cmp(data, root->data);
    if (!result)
    {
        return NULL;
    }

    node_t * temp = NULL;
    if (result < 0)
    {
        if (root->left)
        {
            return node_insert(root->left, data, cmp);
        }
        // else
        temp = calloc(1, sizeof(*temp));
        if (temp)
        {
            temp->data   = data;
            temp->parent = root;
            root->left   = temp;
        }
    }
    else
    {
        if (root->right)
        {
            return node_insert(root->right, data, cmp);
        }
        // else
        temp = calloc(1, sizeof(*temp));
        if (temp)
        {
            temp->data   = data;
            temp->parent = root;
            root->right  = temp;
        }
    }

    return temp;
}

void *
avl_insert(avl_t * tree, void * data)
{
    if (!tree || !data)
    {
        return NULL;
    }

    node_t * new_node = NULL;
    if (!tree->root)
    {
        new_node   = calloc(1, sizeof(*new_node));
        tree->root = new_node;
        if (new_node)
        {
            new_node->data = data;
        }
    }
    else
    {
        new_node = node_insert(tree->root, data, tree->cmp);
    }
    if (!new_node)
    {
        return NULL;
    }
    new_node->height = 1;
    int bf           = 0;

    while (new_node->parent && bf < 2)
    {
        new_node         = new_node->parent;
        int l_bf         = new_node->left ? new_node->left->height : 0;
        int r_bf         = new_node->right ? new_node->right->height : 0;
        int max          = (l_bf < r_bf) ? r_bf : l_bf;
        bf               = abs(r_bf - l_bf);
        new_node->height = max + 1;
    }

    // no rebalancing needed
    if (!new_node->parent && bf < 2)
    {
        return data;
    }

    if (tree->cmp(data, new_node->data) < 0)
    {
        if (tree->cmp(data, new_node->left->data) < 0)
        {
            new_node = rotate_right(&(tree->root), new_node->left, tree->cmp);
            new_node->right->height = new_node->height - 1;
        }
        else
        {
            new_node = rotate_right(
                &(tree->root),
                rotate_left(&(tree->root), new_node->left->right, tree->cmp),
                tree->cmp);
            if (new_node)
            {
                new_node->left->height  = new_node->height;
                new_node->right->height = new_node->height;
                ++new_node->height;
            }
        }
    }
    else
    {
        if (0 < tree->cmp(data, new_node->right->data))
        {
            new_node = rotate_left(&(tree->root), new_node->right, tree->cmp);
            new_node->left->height = new_node->height - 1;
        }
        else
        {
            new_node = rotate_left(
                &(tree->root),
                rotate_right(&(tree->root), new_node->right->left, tree->cmp),
                tree->cmp);
            if (new_node)
            {
                new_node->left->height  = new_node->height;
                new_node->right->height = new_node->height;
                ++new_node->height;
            }
        }
    }
    return data;
}

static node_t *
rotate_right(node_t ** root, node_t * node, cmp_f cmp)
{
    if (!node || node == *root || cmp(node->parent->data, node->data) < 0)
    {
        return node;
    }

    node_t * parent  = node->parent;
    node_t * gparent = parent->parent;
    node_t * child   = node->right;

    node->parent   = gparent;
    node->right    = parent;
    parent->parent = node;
    parent->left   = child;

    if (child)
    {
        child->parent = parent;
    }
    if (gparent)
    {
        if (cmp(parent->data, gparent->data) < 0)
        {
            gparent->left = node;
        }
        else
        {
            gparent->right = node;
        }
    }
    else
    {
        // parent of rotated node is the root; rotated node becomes root
        *root = node;
    }

    return node;
}

static node_t *
rotate_left(node_t ** root, node_t * node, cmp_f cmp)
{
    if (!node || node == *root || 0 < cmp(node->parent->data, node->data))
    {
        return node;
    }

    node_t * parent  = node->parent;
    node_t * gparent = parent->parent;
    node_t * child   = node->left;

    node->parent   = gparent;
    node->left     = parent;
    parent->parent = node;
    parent->right  = child;

    if (child)
    {
        child->parent = parent;
    }
    if (gparent)
    {
        if (cmp(parent->data, gparent->data) < 0)
        {
            gparent->left = node;
        }
        else
        {
            gparent->right = node;
        }
    }
    else
    {
        // parent of rotated node is the root; rotated node becomes root
        *root = node;
    }

    return node;
}

static void
recurse_delete(node_t * node, del_f del)
{
    if (!node)
    {
        return;
    }
    recurse_delete(node->left, del);
    recurse_delete(node->right, del);
    if (del)
    {
        del(node->data);
    }
    free(node);
}

void
avl_destroy(avl_t ** tree, del_f del)
{
    if (!tree || !*tree)
    {
        return;
    }
    recurse_delete((*tree)->root, del);
    free(*tree);
    *tree = NULL;
}

static void
print_trunks(struct trunk * p)
{
    if (!p)
    {
        return;
    }
    print_trunks(p->prev);
    printf("%s", p->str);
}

static void
print_recursive(node_t *       root,
                struct trunk * prev,
                int            is_left,
                print_f        printer)
{
    if (!root)
    {
        return;
    }

    struct trunk this_disp = { prev, "     " };
    const char * prev_str  = this_disp.str;
    print_recursive(root->right, &this_disp, 1, printer);

    if (!prev)
    {
        this_disp.str = "---";
    }
    else if (is_left)
    {
        this_disp.str = ".--";
        prev_str      = "    |";
    }
    else
    {
        this_disp.str = "`--";
        prev->str     = prev_str;
    }

    print_trunks(&this_disp);
    printer(root->data);

    if (prev)
    {
        prev->str = prev_str;
    }
    this_disp.str = "    |";

    print_recursive(root->left, &this_disp, 0, printer);
    if (!prev)
    {
        puts("");
    }
}

void
print_visual(avl_t * tree, print_f printer)
{
    if (!tree)
    {
        return;
    }
    print_recursive(tree->root, NULL, 0, printer);
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
void print_int(void* data)
{
    int value = 0;
    memcpy(&value, data, sizeof(int));
    printf("%d", value);
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

    for (int idx = 0; idx < 5; idx++)
    {
        avl_insert(avl_tree, &data);
        data = data + rand() % 10;
        print_visual(avl_tree, avl_printer);
    }

}
