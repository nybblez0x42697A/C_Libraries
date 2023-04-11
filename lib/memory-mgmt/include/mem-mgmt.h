/**
 * @file
 * @brief Custom memory management system for tracking allocated memory.
 */

#ifndef MEM_MGMT_H
#define MEM_MGMT_H

#define MALLOC(size)      CustomMalloc((size), __FILE__, __LINE__)
#define CALLOC(num, size) CustomCalloc((num), (size), __FILE__, __LINE__)
#define FREE(ptr)         CustomFree((ptr))
#define CLEAN(ptr)        CustomClean((ptr))

#include <pthread.h>
/**
 * @brief Allocate memory and track the allocation in the hash table.
 *
 * This function allocates memory of the specified size using malloc and
 * adds the allocation information to the hash table. The file name and line
 * number of the allocation are also stored.
 *
 * @param size Size of the memory block to be allocated in bytes.
 * @param file File name where the memory block is being allocated.
 * @param line Line number where the memory block is being allocated.
 * @return Pointer to the newly allocated memory block, or NULL on failure.
 */
void * CustomMalloc(size_t size, const char * file, int line);

/**
 * @brief Free memory and update the hash table
 * This function frees the memory block pointed to
 * by the specified pointer and removes the corresponding
 * allocation information from the hash table
 *
 * @param ptr Pointer to the memory block to be freed.
 */
void CustomFree(void * ptr);

/**
 * @brief Allocate and initialize memory and track the allocation in the hash
 * table.
 *
 * This function allocates a memory block of the specified number of elements,
 * each of the specified size, using CustomMalloc. It then initializes the
 * allocated memory block to zero and adds the allocation information to the
 * hash table. The file name and line number of the allocation are also stored.
 *
 * @param num Number of elements to be allocated.
 * @param size Size of each element in bytes.
 * @param file File name where the memory block is being allocated.
 * @param line Line number where the memory block is being allocated.
 * @return Pointer to the newly allocated and initialized memory block, or NULL
 * on failure.
 */
void * CustomCalloc(size_t num, size_t size, const char * file, int line);

/**
 * @brief Free all allocated memory blocks stored in the hash table.
 *
 * This function scans the hash table, frees all memory blocks that have been
 * allocated, and clears the hash table. It should be called when all allocated
 * memory needs to be released, such as before the program exits.
 */
void CustomFreeAll(void);

/**
 * @brief Print memory leaks before the program exits.
 *
 * This function scans the hash table and prints information about memory
 * blocks that have not been freed. It should be called before the program
 * exits to detect memory leaks.
 */
void PrintMemoryLeaks(void);

#endif