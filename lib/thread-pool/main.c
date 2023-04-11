#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "./include/thread_pool.h"
#include "../Signals/include/signal_handlers.h"
#include "../memory-mgmt/include/mem-mgmt.h"

threadpool_t *  temp_pool = NULL;
pthread_mutex_t print_mutex;
pthread_mutex_t cleanup_mutex;
pthread_cond_t  cleanup_cond;

void
print_hello(_Atomic bool * should_shutdown, void * arg)
{
    int id             = *((int *)arg);
    int sleep_duration = rand() % 5 + 1;

    pthread_mutex_lock(&print_mutex);
    printf("Thread %d: Hello, I will sleep for %d seconds.\n",
           id,
           sleep_duration);
    pthread_mutex_unlock(&print_mutex);

    for (int i = 0; i < sleep_duration; i++)
    {
        if (*should_shutdown)
        {
            pthread_mutex_lock(&print_mutex);
            printf("Thread %d: Terminating early due to shutdown request.\n",
                   id);
            pthread_mutex_unlock(&print_mutex);
            return;
        }
        sleep(1);
    }
    pthread_mutex_lock(&print_mutex);
    printf("Thread %d: Goodbye!\n", id);
    pthread_mutex_unlock(&print_mutex);
}

void
main_cleanup()
{

    if (temp_pool)
    {
        thpool_shutdown(temp_pool);
        temp_pool = NULL;
    }
    // exit(0);
}
#ifdef _MAIN_EXCLUDED
int
main()
{

    int   num_threads = 5;
    int   num_jobs    = 10;
    int * job_ids     = CALLOC(num_jobs, sizeof(int));
    clean.items       = CALLOC(2, sizeof(void **));
    pthread_mutex_init(&cleanup_mutex, NULL);
    pthread_mutex_init(&print_mutex, NULL);
    pthread_cond_init(&cleanup_cond, NULL);
    if (!job_ids)
    {
        perror("Failed to allocate memory.");
        return 1;
    }

    srand(time(NULL));

    for (int i = 0; i < num_jobs; i++)
    {
        job_ids[i] = i;
    }
    clean.items[0] = job_ids;
    clean.num_items += 1;
    clean.cleanup_f = main_cleanup;
    install_default_signal_handlers();

    printf("Initializing thread pool with %d threads...\n", num_threads);
    threadpool_t * pool = thpool_init(num_threads);
    temp_pool           = pool;

    if (!pool)
    {
        perror("Failed to allocate memory for pool");
        for (int count = 0; count < num_jobs; count++)
        {
            main_cleanup(NULL);
        }
        return 1;
    }

    printf("Adding %d jobs to the thread pool...\n", num_jobs);
    for (int i = 0; i < num_jobs; i++)
    {
        thpool_add_work(pool, print_hello, &job_ids[i]);
    }

    printf("Waiting for jobs to complete...\n");
    thpool_wait(pool);

    if (pool != NULL)
    {
        printf("Destroying thread pool...\n");
        thpool_destroy(pool);
    }

    printf("Done.\n");

    if (job_ids != NULL)
    {
        free(job_ids);
    }
    PrintMemoryLeaks();
    return 0;
}

#endif
