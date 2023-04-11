#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include "../include/thread_pool.h"

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

    pool->p_thread_object    = calloc(num_threads, sizeof(pthread_t));
    pool->p_job_queue        = calloc(num_threads, sizeof(job_queue_t));
    pool->pb_should_shutdown = calloc(1, sizeof(_Atomic bool));
    pool->pb_should_pause    = calloc(1, sizeof(_Atomic bool));

    if (!pool->p_thread_object || !pool->p_job_queue
        || !pool->pb_should_shutdown || !pool->pb_should_pause)
    {
        perror("Failed to allocate memory for thread objects or job queue");
        return NULL;
    }

    pthread_mutex_init(&pool->resource_lock, NULL);
    pthread_cond_init(&pool->notify_threads, NULL);
    *pool->pb_should_shutdown = false;
    *pool->pb_should_pause    = false;

    for (int i = 0; i < num_threads; i++)
    {
        pthread_create(
            &pool->p_thread_object[i], NULL, thpool_worker, (void *)pool);
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

    pthread_cond_signal(&pool->notify_threads);
    pthread_mutex_unlock(&pool->resource_lock);

    return 0;
}

void *
thpool_worker(void * thpool)
{
    threadpool_t * pool = (threadpool_t *)thpool;
    while (true)
    {
        pthread_mutex_lock(&pool->resource_lock);

        while (*pool->pb_should_pause && !*pool->pb_should_shutdown)
        {
            pthread_cond_wait(&pool->notify_threads, &pool->resource_lock);
        }

        while (pool->jobs_in_queue == 0 && !*pool->pb_should_shutdown)
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
            // pthread_mutex_unlock(&pool->resource_lock);
            continue;
        }

        job_f  function = pool->p_job_queue[pool->job_queue_front].functions;
        void * arg      = pool->p_job_queue[pool->job_queue_front].p_arguments;
        pool->job_queue_front
            = (pool->job_queue_front + 1) % pool->threads_in_pool;
        pool->jobs_in_queue--;
        pool->threads_running++;
        printf("Still Running: %d\n", pool->threads_running);

        pthread_mutex_unlock(&pool->resource_lock);

        function(pool->pb_should_shutdown, arg);

        pthread_mutex_lock(&pool->resource_lock);
        pool->threads_running--;
        printf("Still Running: %d\n", pool->threads_running);
        pthread_cond_signal(&pool->notify_threads);
        pthread_mutex_unlock(&pool->resource_lock);
    }
    printf("I made it.");
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

    free(pool->pb_should_shutdown);
    free(pool->pb_should_pause);
    free(pool->p_thread_object);
    free(pool->p_job_queue);

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
    if (!pool)
    {
        return;
    }
    pthread_mutex_lock(&pool->resource_lock);
    *pool->pb_should_shutdown = true;
    pthread_cond_broadcast(&pool->notify_threads);
    pthread_mutex_unlock(&pool->resource_lock);

    for (int i = 0; i < pool->threads_in_pool; i++)
    {
        pthread_join(pool->p_thread_object[i], NULL);
    }
    thpool_destroy(thpool);
}

int
thpool_num_threads_working(threadpool_t * pool)
{
    if (!pool)
    {
        return -1;
    }
    int num_threads_working;

    pthread_mutex_lock(&pool->resource_lock);
    num_threads_working = pool->threads_running;
    pthread_mutex_unlock(&pool->resource_lock);

    return num_threads_working;
}
