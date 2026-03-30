#include <my_header.h>

pthread_mutex_t lock;
pthread_cond_t cond;

void* pthread_func(void* arg){
    sleep(1);
    // 先睡一会，子线程再上（为了让主线程先拿到锁然后因为条件不满足再把自己阻塞住）
    int ret = pthread_mutex_lock(&lock);
    THREAD_ERROR_CHECK(ret, "pthread_mutex_lock");

    // 访问共享资源
    printf("I am son\n");

    // 先让子线程解锁
    ret = pthread_mutex_unlock(&lock);
    THREAD_ERROR_CHECK(ret, "phread_mutex_unlock");
    pthread_cond_signal(&cond); // 唤醒阻塞的main线程，若无这步，则main会主动阻塞到永远

    pthread_exit((void*)NULL);
}

int main(int argc, char* argv[]){                                  
    // 初始化互斥锁
    int ret = pthread_mutex_init(&lock, NULL);
    THREAD_ERROR_CHECK(ret, "pthread_mutex_init");

    // 初始化条件变量
    ret = pthread_cond_init(&cond, NULL);
    THREAD_ERROR_CHECK(ret, "pthread_cond_init");

    // 创建子进程
    pthread_t thread_id;
    ret = pthread_create(&thread_id, NULL, pthread_func, NULL);
    THREAD_ERROR_CHECK(ret, "pthread_create");
    
    ret = pthread_mutex_lock(&lock);
    THREAD_ERROR_CHECK(ret, "pthread_mutex_lock");

    printf("begin wait\n");
    // 因为没有满足条件，而睡眠
    ret = pthread_cond_wait(&cond, &lock);
    THREAD_ERROR_CHECK(ret, pthread_cond_wait);

    printf("I am main, condition is ok\n");

    // 解锁
    ret = pthread_mutex_unlock(&lock);
    THREAD_ERROR_CHECK(ret, "pthread_mutex_unlock");

    // 主线程等子线程
    ret = pthread_join(thread_id, NULL);
    THREAD_ERROR_CHECK(ret, "pthread_join");

    // 最后一步，需要回收锁的资源
    ret = pthread_mutex_destroy(&lock);
    THREAD_ERROR_CHECK(ret, "pthread_mutex_destroy");
    // 条件变量的销毁
    ret = pthread_cond_destroy(&cond);
    THREAD_ERROR_CHECK(ret, "pthread_cond_destroy");
    
    return 0;
}

