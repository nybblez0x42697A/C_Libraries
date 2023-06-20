#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <stdbool.h>
#include <pthread.h>

typedef void (*job_f)(void * arg);

typedef struct threadpool threadpool_t;
pthread_mutex_t           cleanup_mutex;
pthread_mutex_t           print_mutex;

pthread_cond_t cleanup_cond;

threadpool_t * thpool_init(int num_threads);
int  thpool_add_work(threadpool_t * pool, job_f function, void * arg);
void thpool_wait(threadpool_t * pool);
void thpool_destroy(void * thpool);
void thpool_pause(threadpool_t * pool);
void thpool_resume(threadpool_t * pool);
int  thpool_num_threads_working(threadpool_t * pool);
void thpool_shutdown(void * thpool);

#endif // THREAD_POOL_H
