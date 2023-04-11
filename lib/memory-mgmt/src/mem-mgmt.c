/**
 * @file
 * @brief Custom memory management system for tracking allocated memory.
 */

#include "../include/mem-mgmt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Structure for holding information about each allocated memory block.
 */
typedef struct MemoryBlock_Tag
{
    void *       ptr;  /**< Pointer to the allocated memory block. */
    size_t       size; /**< Size of the allocated memory block in bytes. */
    const char * file; /**< File name where the memory block was allocated. */
    int          line; /**< Line number where the memory block was allocated. */
    struct MemoryBlock_Tag * next; /**< Next MemoryBlock in the linked list. */
} MemoryBlock_T;

/** @brief Hash table size parameter. */
#define HASH_TABLE_SIZE 1024

/** @brief Global hash table for keeping track of allocated memory. */
static MemoryBlock_T * g_memory_table[HASH_TABLE_SIZE] = { 0 };

/** @brief Mutex for synchronizing access to the hash table. */
static pthread_mutex_t g_memory_mutex = PTHREAD_MUTEX_INITIALIZER;

static size_t
hash_ptr(void * ptr)
{
    return ((size_t)ptr) % HASH_TABLE_SIZE;
}

void *
CustomMalloc(size_t size, const char * file, int line)
{
    void * ptr = malloc(size);
    if (ptr)
    {
        MemoryBlock_T * new_block = malloc(sizeof(MemoryBlock_T));
        if (new_block)
        {
            new_block->ptr  = ptr;
            new_block->size = size;
            new_block->file = file;
            new_block->line = line;

            size_t hash_value = hash_ptr(ptr);

            pthread_mutex_lock(&g_memory_mutex);
            new_block->next            = g_memory_table[hash_value];
            g_memory_table[hash_value] = new_block;
            pthread_mutex_unlock(&g_memory_mutex);
        }
    }

    return ptr;
}

void
CustomFree(void * ptr)
{
    if (!ptr)
    {
        return;
    }

    size_t hash_value = hash_ptr(ptr);

    pthread_mutex_lock(&g_memory_mutex);
    MemoryBlock_T * prev    = NULL;
    MemoryBlock_T * current = g_memory_table[hash_value];
    while (current)
    {
        if (current->ptr == ptr)
        {
            if (prev)
            {
                prev->next = current->next;
            }
            else
            {
                g_memory_table[hash_value] = current->next;
            }
            free(current->ptr);
            free(current);
            break;
        }
        prev    = current;
        current = current->next;
    }
    pthread_mutex_unlock(&g_memory_mutex);
}

void *
CustomCalloc(size_t num, size_t size, const char * file, int line)
{
    size_t total_size = num * size;
    void * ptr        = CustomMalloc(total_size, file, line);
    if (ptr)
    {
        memset(ptr, 0, total_size);
    }

    return ptr;
}

void
PrintMemoryLeaks(void)
{
    pthread_mutex_lock(&g_memory_mutex);
    for (size_t i = 0; i < HASH_TABLE_SIZE; i++)
    {
        MemoryBlock_T * current = g_memory_table[i];
        while (current)
        {
            printf("Leak: %zu bytes at %p, allocated in %s:%d\n",
                   current->size,
                   current->ptr,
                   current->file,
                   current->line);
            current = current->next;
        }
    }
    pthread_mutex_unlock(&g_memory_mutex);
}

void
CustomClean(void)
{
    pthread_mutex_lock(&g_memory_mutex);
    for (size_t i = 0; i < HASH_TABLE_SIZE; i++)
    {
        MemoryBlock_T * current = g_memory_table[i];
        while (current)
        {
            MemoryBlock_T * next_block = current->next;
            free(current->ptr);
            free(current);
            current = next_block;
        }
        g_memory_table[i] = NULL;
    }
    pthread_mutex_unlock(&g_memory_mutex);
}

/* ----------------------------------------------------------------------------

Example usage
--------------------------------------------------------------------------*/
#ifdef _MAIN_EXCLUDED
int
main(void)
{
    /* Allocate memory using the custom functions */
    int *  arr = MALLOC(10 * sizeof(int));
    char * str
        = CALLOC(20, sizeof(char)); /* Intentionally skip freeing 'str' to
                                       demonstrate memory leak detection */
    FREE(arr);
    strcpy(str, "hello");

    /* Print memory leaks before the program exits */
    PrintMemoryLeaks();

    return 0;
}
#endif