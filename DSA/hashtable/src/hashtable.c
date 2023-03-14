/** @file hashtable.c
 *
 * @brief A basic data agnostic hashtable library.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define TABLE_SIZE 10

typedef struct ht_bucket
{
    char *           key;
    char *           value;
    struct ht_bucket * next;
} ht_bucket_t;

typedef struct
{
    ht_bucket_t ** entries;
} htable_t;

/* Create a new hash table. */
htable_t *
ht_create()
{
    htable_t * hashtable = NULL;
    int    i;

    if ((hashtable = malloc(sizeof(htable_t))) == NULL)
    {
        return NULL;
    }

    if ((hashtable->entries = malloc(sizeof(ht_bucket_t *) * TABLE_SIZE)) == NULL)
    {
        return NULL;
    }

    for (i = 0; i < TABLE_SIZE; i++)
    {
        hashtable->entries[i] = NULL;
    }

    return hashtable;
}

/* Hash a string for a particular hash table. */
int
ht_hash(htable_t * hashtable, char * key)
{
    unsigned long int hashval = 0;
    int               i       = 0;

    /* Convert our string to an integer */
    while (hashval < ULONG_MAX && i < strlen(key))
    {
        hashval = hashval << 8;
        hashval += key[i];
        i++;
    }

    return hashval % TABLE_SIZE;
}

/* Create a key-value pair. */
ht_bucket_t *
ht_newpair(char * key, char * value)
{
    ht_bucket_t * newpair;

    if ((newpair = malloc(sizeof(ht_bucket_t))) == NULL)
    {
        return NULL;
    }

    if ((newpair->key = strdup(key)) == NULL)
    {
        return NULL;
    }

    if ((newpair->value = strdup(value)) == NULL)
    {
        return NULL;
    }

    newpair->next = NULL;

    return newpair;
}

/* Insert a key-value pair into a hash table. */
void
ht_set(htable_t * hashtable, char * key, char * value)
{
    int       bin     = 0;
    ht_bucket_t * newpair = NULL;
    ht_bucket_t * next    = NULL;
    ht_bucket_t * last    = NULL;

    bin = ht_hash(hashtable, key);

    next = hashtable->entries[bin];

    while (next != NULL && next->key != NULL && strcmp(key, next->key) > 0)
    {
        last = next;
        next = next->next;
    }

    /* There's already a pair.  Let's replace that string. */
    if (next != NULL && next->key != NULL && strcmp(key, next->key) == 0)
    {

        free(next->value);
        next->value = strdup(value);

        /* Nope, could't find it.  Time to grow a pair. */
    }
    else
    {
        newpair = ht_newpair(key, value);

        /* We're at the start of the linked list in this bin. */
        if (next == hashtable->entries[bin])
        {
            newpair->next           = next;
            hashtable->entries[bin] = newpair;

            /* We're at the end of the linked list in this bin. */
        }
        else if (next == NULL)
        {
            last->next = newpair;

            /* We're in the middle of the list. */
        }
        else
        {
            newpair->next = next;
            last->next    = newpair;
        }
    }
}    
    /* Retrieve a key-value pair from a hash table. */
    char * ht_get(htable_t * hashtable, char * key)
    {
        int       bin = 0;
        ht_bucket_t * pair;

        bin = ht_hash(hashtable, key);

        /* Step through the bin, looking for our value. */
        pair = hashtable->entries[bin];
        while (pair != NULL && pair->key != NULL && strcmp(key, pair->key) > 0)
        {
            pair = pair->next;
        }

        /* Did we actually find anything? */
        if (pair == NULL || pair->key == NULL || strcmp(key, pair->key) != 0)
        {
            return NULL;
        }
        else
        {
            return pair->value;
        }
    }