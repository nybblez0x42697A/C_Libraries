#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief A function pointer to a custom-defined delete function
 *        required to support deletion/memory deallocation of
 *        arbitrary data types. For simple data types, this function
 *        pointer can simply point to the free function.
 *
 */
typedef void (*del_f)(void * data);

/**
 * @brief A function pointer to a custom-defined comparison function
 *        required to support comparing arbitrary data types. The
 *        user of the library defines this function in their program,
 *        and it's return values should work similar to strcmp in that
 * @returns -1 if lhs < rhs
 *          0 if lhs == rhs
 *          1 if lhs > rhs
 *
 */
typedef int (*cmp_f)(void * lhs, void * rhs);

/**
 * @brief A function pointer to a custom-defined action function that takes a
 *        pointer to the data value in a node as the first parameter. It takes
 * an optional second parameter. The second parameter is provided to as an
 * option to the library user depending on what they write the action function
 * to do.
 *
 *        The action function is used in the linked_list_for_each() function to
 * allow a function with this prototype to be called on every node in the linked
 * list.
 *
 *        i.e. Common uses for this might be a function that prints each value
 * in the list.
 *
 */
typedef void (*action_f)(void * item, void * ctx);

/**
 * @brief A struct that defines a node in the linked list.
 *
 */
typedef struct ll_node_t
{
    void *             data;
    struct ll_node_t * next;
} ll_node_t;

/**
 * @brief A struct that defines a pointer to the head node as well as
 *        additional metadata your linked list implementation contains.
 *
 */
typedef struct linked_list_t
{
    ll_node_t * head;
    ll_node_t * tail;
    del_f       delete_func;
    cmp_f       compare_func;
    size_t      size;
} linked_list_t;

/**
 * @brief Creates a newly allocated linked list
 *
 * The custom del and cmp functions are tailored to the data type or data
 * structure being stored in the linked list.  For simple data types, passing
 * the standard free function as the del parameter will usually suffice. These
 * function pointers allow the library user to store arbitrary data in the
 * linked list.
 *
 * Asserts that the cmp function is not NULL.
 * If NULL is passed as the del parameter, implementation must set free as the
 * default.
 *
 * @param del a function pointer to a custom free function tailored to
 * @param cmp a function pointer to a custom compare function
 * @return linked_list_t* a pointer to an allocated linked list object
 */
linked_list_t * linked_list_new(del_f del, cmp_f cmp);

/**
 * @brief Destroy the list and all nodes it contains, deallocating any memory
 *        that was allocated.
 *
 * Asserts that list is not NULL, however *list == NULL is safe.
 * List pointer is set to NULL after memory is deallocated to
 * mitgate chance of Use-after-free bugs.
 *
 * @param list a reference to a pointer to an allocated list.
 */
void linked_list_delete(linked_list_t ** list);

/**
 * @brief Returns the number of nodes in the linked list
 *
 * Asserts that the list pointer is not NULL.
 *
 * @param list a pointer to an allocated linked list object
 * @return size_t size, the number of nodes in the linked list if list != NULL.
 *         else returns -1.
 */
size_t linked_list_size(linked_list_t * list);

/**
 * @brief Prepends a node before the given index in the linked list.
 *        The index starts at 0.
 *
 * A new node will be allocated with the data passed as a parameter to the
 * function.
 *
 * If 0 is provided as an index, a node will be prepended as the new head node,
 * and the existing head node will become the next node in the list. If 1 is
 * provided as an index, a node will be inserted between the existing head node
 * and it's next node, with that existing next node becoming the new node's next
 * node.
 *
 * Asserts that the list pointer is not NULL.
 * Asserts that index <= list size. If index == size, the element will be
 * appended.
 *
 * @param list a pointer to an allocated linked list object
 * @param index the index of the reference node to which this new node will be
 *              prepended
 * @param data the data that will be placed in the newly allocated node
 * @return 0 if successful, -1 if list == NULL, -2 if index > list size
 */
size_t linked_list_insert_before(linked_list_t * list,
                                 size_t          index,
                                 void *          data);

/**
 * @brief Appends a node to the tail of the linked list
 *
 * A new node will be allocated with the data passed as a parameter to this
 * function.
 *
 * Asserts that the list pointer is not NULL.
 *
 * @param list a pointer to an allocated linked list object
 * @param data the data that will be placed in the newly allocated node
 * @return 0 on success, -1 if the list pointer is NULL.
 */
size_t linked_list_append(linked_list_t * list, void * data);

/**
 * @brief Finds the index of the first node matching the value parameter
 *        starting at index given by the start parameter.  Matches are
 *        defined by the custom comparison function that was passed to the cmp
 *        parameter in `linked_list_new`.
 *
 * A start index of 0 means search from the beginning of the list.
 *
 * Asserts that the list pointer is not NULL.
 *
 * @param list a pointer to an allocated linked list object
 * @param value a pointer to the value being matched after the given start index
 * @param start the 0-indexed index within the list where matching should begin
 * @return size_t the index in the list where the match was found, or (size_t)
 *         -1 if not found
 */
size_t linked_list_find_first_after(linked_list_t * list,
                                    void *          value,
                                    size_t          start);

/**
 * @brief Returns the data pointer in the 0-indexed node at index starting from
 * the head
 *
 * Asserts that the list pointer is not NULL.
 * Asserts that index is less than list size (since index starts at 0).
 *
 * @param list a pointer to an allocated list
 * @param index the index of the node being accessed
 * @return void * return_value. a pointer to the data contained in the node at the given index.
 *         If list == NULL or index >= list size, return NULL.
 */
void * linked_list_at(linked_list_t * list, size_t index);

/**
 * @brief Delete a node in the linked list at a given index. Index starts at 0.
 *
 * Asserts that the list pointer is not NULL.
 * Asserts that index is less than list size (since index starts at 0).
 *
 * @param list a pointer to an allocated linked list object
 * @param index the index of the node to be deleted, where index=0 is the head
 * node
 */
void linked_list_del_at(linked_list_t * list, size_t index);

/**
 * @brief Calls the function specified in the action parameter on every node in
 *        the linked list.
 *
 * Asserts that list pointer is NOT NULL.
 *
 * @param list a pointer to an allocated linked list object
 * @param action a function pointer to a user-specified function that will be
 * called on each node in the list. An example function might print the data in
 * a node.
 *
 * @param action_ctx a pointer to the second parameter defined by the action
 * function. (The first parameter to action is the data pointer in a list node.)
 *                   An action function that doesn't require the second
 * parameter can simply set this to NULL.
 */
void linked_list_for_each(linked_list_t * list,
                          action_f        action,
                          void *          action_ctx);

#endif
