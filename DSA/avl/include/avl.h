/** @file avl.h
 *
 * @brief Auto-leveling BST, threadsafe, using void pointers
 *        Requires pthread library during compilation
 */
#ifndef AVL_H
#define AVL_H

#include <stdlib.h>
#include <string.h>

/**
 * @brief Struct that defines pointer to root node, pthread lock,
 *        and del_f that knows how to delete void*
 */
typedef struct  avl_t avl_t;
typedef struct node_t node_t;
typedef struct trunk trunk_t;

/**
 * @brief User provided function to properly delete inserted data
 */
typedef void (*del_f)(void * data);

/**
 * @brief User provided function to properly compare void *
 * @param[in] arg1 Void pointer of data to compare as
 * @param[in] arg2 Void pointer to data to compare to
 *
 * @return < 0; arg1 < arg2
 * @return 0; arg1 == arg2
 * @return > 0; arg1 > arg2
 */
typedef int (*cmp_f)(const void *, const void *);

/**
 * @brief User provided function to visually represent void *data
 */
typedef void (*print_f)(void *);

/**
 * @brief Creates AVL-Tree
 *
 * @param[in] cmp_f Compare function used to properly insert nodes
 *
 * @return A new AVL-Tree; else NULL
 */
avl_t * avl_create(cmp_f);

/**
 * @brief Adds element to tree
 *
 * @param[in] avl_t Tree pointer to add data too
 * @param[in] void Data pointer to be added
 *
 * @return Address of void * on success, else NULL
 */
void * avl_insert(avl_t *, void *);

/**
 * @brief Searches AVL for value
 *
 * @param[in] avl_t Tree pointer to search
 * @param[in] void Data pointer to search for
 * @param[in] cmp_f Comparison function to use for searching
 *
 * @return Pointer to requested data or NULL
void *avl_search(avl_t *, const void *, cmp_f);
*/

/**
 * @brief Destroys the AVL tree and uses del_f to free void *
 *
 * @param[in,out] avl_t Address of where tree pointer is stored
 * @param[in] del_f Delete function to use, else NULL to not delete
 */
void avl_destroy(avl_t **, del_f);

void print_visual(avl_t *, print_f);

#endif /* AVL_H */

/*** end of file ***/
