#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdarg.h>

#include "../include/thread_pool.h"

#define ERROR_MESSAGE_LENGTH 24

typedef struct job_queue_t
{
    job_f  functions;
    void * p_arguments;
} job_queue_t;

typedef struct threadpool
{
    _Atomic bool *  pb_should_shutdown;
    _Atomic bool *  pb_should_pause;
    pthread_t *     p_thread_object;
    pthread_mutex_t resource_lock;
    pthread_cond_t  notify_threads;
    int             threads_in_pool;
    int             thread_capacity;
    int             threads_running;
    int             job_queue_front;
    int             job_queue_rear;
    int             jobs_in_queue;
    job_queue_t *   p_job_queue;
} threadpool_t;

void * thpool_worker(void * pool);

threadpool_t *
thpool_init(int num_threads)
{
    threadpool_t * pool = calloc(1, sizeof(threadpool_t));
    if (!pool)
    {
        perror("Failed to allocate memory for threadpool_t");
        return NULL;
    }

    pool->threads_in_pool = num_threads;
    pool->thread_capacity = num_threads;

    pool->p_thread_object    = calloc(num_threads, sizeof(pthread_t));
    pool->p_job_queue        = calloc(num_threads, sizeof(job_queue_t));
    pool->pb_should_shutdown = calloc(1, sizeof(_Atomic bool));
    pool->pb_should_pause    = calloc(1, sizeof(_Atomic bool));

    if (!(pool->p_thread_object) || !(pool->p_job_queue)
        || !(pool->pb_should_shutdown) || !(pool->pb_should_pause))
    {
        perror("Failed to allocate memory for thread objects or job queue");
        return NULL;
    }

    pthread_mutex_init(&pool->resource_lock, NULL);
    pthread_cond_init(&pool->notify_threads, NULL);
    *pool->pb_should_shutdown = false;
    *pool->pb_should_pause    = false;

    for (int idx = 0; idx < num_threads; idx++)
    {
        pthread_create(
            &pool->p_thread_object[idx], NULL, thpool_worker, (void *)pool);
    }

    return pool;
}

int
thpool_add_work(threadpool_t * pool, job_f function, void * arg)
{
    if (!pool || !function)
    {
        return -1;
    }

    pthread_mutex_lock(&pool->resource_lock);

    if (pool->jobs_in_queue == pool->threads_in_pool)
    {
        perror("Job queue is full");
        pthread_mutex_unlock(&pool->resource_lock);
        return -1;
    }

    pool->p_job_queue[pool->job_queue_rear].functions   = function;
    pool->p_job_queue[pool->job_queue_rear].p_arguments = arg;
    pool->job_queue_rear = (pool->job_queue_rear + 1) % pool->threads_in_pool;
    pool->jobs_in_queue++;

    pthread_cond_broadcast(&pool->notify_threads);
    pthread_mutex_unlock(&pool->resource_lock);

    return 0;
}

void *
thpool_worker(void * thpool)
{
    threadpool_t * pool = (threadpool_t *)thpool;
    while (!(*pool->pb_should_shutdown))
    {
        pthread_mutex_lock(&pool->resource_lock);

        while ((*pool->pb_should_pause || pool->jobs_in_queue == 0)
               && !(*pool->pb_should_shutdown))
        {
            pthread_cond_wait(&pool->notify_threads, &pool->resource_lock);
        }

        if (*pool->pb_should_shutdown)
        {
            pthread_mutex_unlock(&pool->resource_lock);
            break;
        }

        // Check the pause flag
        if (*pool->pb_should_pause)
        {
            pthread_mutex_unlock(&pool->resource_lock);
            continue;
        }

        job_queue_t job = pool->p_job_queue[pool->job_queue_front];

        pool->job_queue_front
            = (pool->job_queue_front + 1) % pool->threads_in_pool;
        pool->jobs_in_queue--;
        pool->threads_running++;

        pthread_mutex_unlock(&pool->resource_lock);

        job.functions(job.p_arguments);

        pthread_mutex_lock(&pool->resource_lock);
        pool->threads_running--;

        pthread_cond_broadcast(&pool->notify_threads);
        pthread_mutex_unlock(&pool->resource_lock);
    }

    pthread_mutex_lock(&pool->resource_lock);
    pool->threads_in_pool--;
    pthread_mutex_unlock(&pool->resource_lock);

    pthread_exit(NULL);
}

void
thpool_wait_for_jobs(threadpool_t * pool)
{
    if (!pool)
    {
        return;
    }

    pthread_mutex_lock(&pool->resource_lock);

    while (pool->jobs_in_queue > 0)
    {
        pthread_cond_wait(&pool->notify_threads, &pool->resource_lock);
    }

    pthread_mutex_unlock(&pool->resource_lock);
}

void
thpool_wait(threadpool_t * pool)
{
    if (!pool)
    {
        return;
    }

    pthread_mutex_lock(&pool->resource_lock);

    while (pool->jobs_in_queue > 0 || pool->threads_running > 0)
    {
        // Added this block to wake up sleeping threads when there are jobs in
        // the queue.
        if (pool->jobs_in_queue > 0 && pool->threads_running == 0)
        {
            pthread_cond_broadcast(&pool->notify_threads);
        }

        pthread_cond_wait(&pool->notify_threads, &pool->resource_lock);
    }

    pthread_mutex_unlock(&pool->resource_lock);
}

void
thpool_destroy(void * thpool)
{
    threadpool_t * pool = (threadpool_t *)thpool;
    if (!pool)
    {
        return;
    }

    free(pool->p_thread_object);
    free(pool->p_job_queue);

    free(pool->pb_should_shutdown);
    free(pool->pb_should_pause);

    pthread_mutex_destroy(&pool->resource_lock);
    pthread_cond_destroy(&pool->notify_threads);
    free(pool);
}

void
thpool_pause(threadpool_t * pool)
{
    if (!pool)
    {
        return;
    }
    pthread_mutex_lock(&pool->resource_lock);
    *pool->pb_should_pause = true;
    pthread_cond_broadcast(&pool->notify_threads);
    pthread_mutex_unlock(&pool->resource_lock);
}

void
thpool_resume(threadpool_t * pool)
{
    if (!pool)
    {
        return;
    }
    pthread_mutex_lock(&pool->resource_lock);
    *pool->pb_should_pause = false;
    pthread_cond_broadcast(&pool->notify_threads);
    pthread_mutex_unlock(&pool->resource_lock);
}

void
thpool_shutdown(void * thpool)
{
    threadpool_t * pool = (threadpool_t *)thpool;
    if (!pool || *pool->pb_should_shutdown)
    {
        return;
    }
    pthread_mutex_lock(&pool->resource_lock);
    *pool->pb_should_shutdown = true;
    pthread_cond_broadcast(&pool->notify_threads);
    pthread_mutex_unlock(&pool->resource_lock);

    int    join_result = 0;
    void * pthread_ret = NULL;
    for (int idx = 0; idx < pool->thread_capacity; idx++)
    {

        join_result = pthread_join(pool->p_thread_object[idx], &pthread_ret);

        if (join_result != 0)
        {
            char error_msg[ERROR_MESSAGE_LENGTH];
            snprintf(error_msg,
                     ERROR_MESSAGE_LENGTH,
                     "Error joining thread %d",
                     idx);
            perror(error_msg);
        }
        free(pthread_ret);
    }

    while (pool->threads_in_pool > 0)
    {
        usleep(1000); // sleep for 1ms
    }
    thpool_destroy(pool);
}

int
thpool_num_threads_working(threadpool_t * pool)
{
    if (!pool)
    {
        return -1;
    }
    int num_threads_working = 0;

    pthread_mutex_lock(&pool->resource_lock);
    num_threads_working = pool->threads_running;
    pthread_mutex_unlock(&pool->resource_lock);

    return num_threads_working;
}

#ifndef _MAIN_EXCLUDED_
// Define the work that will be done by the thread.
// Thread safe printf
void
safe_printf(void * format)
{

    pthread_mutex_t print_lock;

    pthread_mutex_init(&print_lock, NULL);

    // Lock
    pthread_mutex_lock(&print_lock);

    // Write
    fprintf(stdout, "Hey %s!\n", (char *)format);

    // Unlock
    pthread_mutex_unlock(&print_lock);
    pthread_mutex_destroy(&print_lock);
}

int
main()
{
    // Create a threadpool with 4 threads.
    threadpool_t * pool = thpool_init(4);
    if (!pool)
    {
        fprintf(stderr, "Failed to create thread pool.\n");
        return 1;
    }

    // Add some jobs to the pool.
    const char * names[] = { "Alice", "Bob", "Charlie", "Dave" };
    for (int idx = 0; idx < 4; idx++)
    {
        if (thpool_add_work(pool, safe_printf, (void *)names[idx]) != 0)
        {
            fprintf(stderr, "Failed to add job to thread pool.\n");
            thpool_shutdown(pool);
            return 1;
        }
    }

    // Wait for all jobs to finish.
    thpool_wait(pool);

    // Shutdown the thread pool.
    thpool_shutdown(pool);

    return 0;
}
#endif