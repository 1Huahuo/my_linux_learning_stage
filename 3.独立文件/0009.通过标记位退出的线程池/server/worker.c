#include <my_header.h>
#include "worker.h"

void* thread_func(void* arg){
    thread_pool_t* pool = (thread_pool_t*)arg;
    
    while(1){
        pthread_mutex_lock(&pool->mutex);

        while(pool->queue.size == 0 && pool->exit_flag == 0)
            pthread_cond_wait(&pool->cond, &pool->mutex);

        if(pool->exit_flag == 1){
            pthread_mutex_unlock(&pool->mutex);
            pthread_exit(NULL);
        }

        int client_fd = deQueue(&pool->queue);

        pthread_mutex_unlock(&pool->mutex);

        send_file(client_fd);

        close(client_fd);
    }

    return NULL;
}
