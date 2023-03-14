#include "../include/ll.h"

/**
 * @brief Internal function that Creates a sliding node for traversing the
 *        linked list. Asserts list parameter is not NULL.
 *
 * @param list
 * @return void* p_sliding_node. The creataed sliding node.
 */
void * linked_list_create_sliding_node(linked_list_t * list);

linked_list_t *
linked_list_new(del_f del, cmp_f cmp)
{
    linked_list_t * new_list = NULL;

    if (cmp != NULL)
    {
        new_list               = calloc(1, sizeof(linked_list_t));
        new_list->compare_func = cmp;
        new_list->head         = calloc(1, sizeof(ll_node_t));

        if (del == NULL)
        {
            new_list->delete_func = free;
        }

        else
        {
            new_list->delete_func = del;
        }
    }

    return new_list;
}

void
linked_list_delete(linked_list_t ** list)
{
    assert(list != NULL);
    ll_node_t *p_sliding_node, *p_sliding_node2;
    p_sliding_node = (*list)->head;

    while (p_sliding_node != NULL)
    {

        p_sliding_node2 = p_sliding_node->next;
        (*list)->delete_func(p_sliding_node->data);
        (*list)->delete_func(p_sliding_node);
        p_sliding_node = p_sliding_node2;
    }

    (*list)->delete_func(*list);
}

size_t
linked_list_size(linked_list_t * list)
{
    size_t size = 0;

    if (NULL != list)
    {
        size = list->size;
    }
    else
    {
        size = -1;
    }
    return size;
}

size_t
linked_list_insert_before(linked_list_t * list, size_t index, void * data)
{
    size_t return_value = -1;

    if (NULL != list)
    {
        return_value = 0;
        if (!(index <= linked_list_size(list)))
        {

            ll_node_t * p_sliding_node = linked_list_create_sliding_node(list);

            ll_node_t * p_inserted_node = calloc(1, sizeof(ll_node_t));
            p_inserted_node->data       = data;
            p_inserted_node->next       = NULL;
            size_t count                = 0;

            if (index == 0)
            {
                p_inserted_node->next = list->head;
                list->head            = p_inserted_node;
                list->size++;
            }

            else if (index == linked_list_size(list))
            {
                list->tail->next = p_inserted_node;
                list->tail       = p_inserted_node;
                list->size++;
            }

            else
            {

                while (count < (index - 1))
                {

                    p_sliding_node = p_sliding_node->next;
                    count++;
                }

                p_inserted_node->next = p_sliding_node->next;
                p_sliding_node->next  = p_inserted_node;
                list->size++;
            }
        }

        else
        {
            return_value = -2;
        }
    }
    return return_value;
}

size_t
linked_list_append(linked_list_t * list, void * data)
{
    size_t return_value = -1;

    if (NULL != list)
    {
        ll_node_t * p_appended_node = calloc(1, sizeof(ll_node_t));

        p_appended_node->next = NULL;
        p_appended_node->data = data;

        if (list->head->data == NULL)
        {
            list->head->data = data;
            list->head->next = NULL;
            list->size++;
        }

        else
        {
            list->tail->next = p_appended_node;
            list->size++;
        }
    }

    return return_value;
}

size_t
linked_list_find_first_after(linked_list_t * list, void * value, size_t start)
{

    size_t return_value = -1;
    int    count        = 0;

    if (NULL != list)
    {
        ll_node_t * p_sliding_node = linked_list_create_sliding_node(list);
        size_t      cmp_result     = NULL;

        for (; count < linked_list_size(list); count++)
        {
            if (count >= start)
            {
                cmp_result = list->compare_func(*(int *)p_sliding_node->data,
                                                *(int *)value);
                if (cmp_result == 0)
                {
                    return_value = count;
                    break;
                }
            }
            p_sliding_node = p_sliding_node->next;
        }
    }

    return return_value;
}
void *
linked_list_at(linked_list_t * list, size_t index)
{
    size_t count        = 0;
    void * return_value = NULL;

    if (NULL != list)
    {
        ll_node_t * p_sliding_node = NULL;
        p_sliding_node             = linked_list_create_sliding_node(list);

        if (index < linked_list_size(list))
        {
            while (count < index)
            {

                p_sliding_node = p_sliding_node->next;
                count++;
            }

            if (NULL != p_sliding_node->data)
            {
                return_value = p_sliding_node->data;
            }
        }
    }

    return return_value;
}

void
linked_list_del_at(linked_list_t * list, size_t index)
{

    if (NULL != list)
    {
        if (index <= linked_list_size(list))
        {
            size_t count = 0;

            ll_node_t * p_sliding_node = linked_list_create_sliding_node(list);

            ll_node_t * p_sliding_node2 = linked_list_create_sliding_node(list);

            if (list->head->data != NULL)
            {
                while (count < index)
                {

                    p_sliding_node  = p_sliding_node2;
                    p_sliding_node2 = p_sliding_node2->next;
                    count++;
                }

                if (index == 0)
                {
                    p_sliding_node = list->head->next;
                    list->delete_func(list->head->data);
                    list->delete_func(list->head);
                    list->head = p_sliding_node;
                }

                else
                {
                    p_sliding_node->next = p_sliding_node2->next;
                    list->delete_func(p_sliding_node2->data);
                    list->delete_func(p_sliding_node2);
                }
            }
        }
    }
}

void
linked_list_for_each(linked_list_t * list, action_f action, void * action_ctx)
{

    if (NULL != list)
    {
        ll_node_t * p_sliding_node = linked_list_create_sliding_node(list);

        while (p_sliding_node->next != NULL)
        {
            action(p_sliding_node->data, action_ctx);
            p_sliding_node = p_sliding_node->next;
        }
    }
}

static void *
linked_list_create_sliding_node(linked_list_t * list)
{
    ll_node_t * p_sliding_node;
    p_sliding_node = list->head;

    return p_sliding_node;
}
