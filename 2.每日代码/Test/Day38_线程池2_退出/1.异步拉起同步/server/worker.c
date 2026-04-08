#include <my_header.h>
#include "worker.h"

void cleanup_lock(void* p){ // pthread_cleanup_push的第一个参数即处理函数返回值为void，参数为void*
    pthread_mutex_t* mutex = (pthread_mutex_t*)p;
    pthread_mutex_unlock(mutex);
}

void* thread_func(void* arg){
    thread_pool_t* pool = (thread_pool_t*)arg;
    
    while(1){
        int client_fd; // 整个外层while内的局部变量
        pthread_mutex_lock(&pool->mutex);
        pthread_cleanup_push(cleanup_lock, &pool->mutex);

        while(pool->queue.size == 0)
            pthread_cond_wait(&pool->cond, &pool->mutex);

        /*int */client_fd = deQueue(&pool->queue); // 此时不能在此定义，因为push与pop之间是一个花括号，里面定义的为局部变量 


        /* pthread_mutex_unlock(&pool->mutex); */
        pthread_cleanup_pop(1); // 这行代码无论线程有没有被取消都会执行，而上面一行若是线程被取消则不会执行，会造成死锁

        send_file(client_fd);

        close(client_fd); // 客户端非持续请求,所以对于服务器而言传输一次数据就可以关闭客户端文件描述符
    }
    
    return NULL;
}
