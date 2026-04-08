#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#include <my_header.h>
#include "queue.h"
#include "worker.h"

typedef struct thread_pool{
    int thread_num;
    pthread_t* thread_id_arr;
    queue_t queue;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
}thread_pool_t;

// 初始化线程池中的数据
void init_thread_pool(thread_pool_t* pool, int num);

#endif

// pool 游泳池；池塘，水坑；一小片（液体或光）
