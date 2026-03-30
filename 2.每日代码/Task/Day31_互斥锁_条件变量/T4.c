#include <my_header.h>

pthread_mutex_t mutex;

pthread_cond_t cond_ab, cond_bc/*, cond_ca*/; // 同一线程内部，没必要搞同步 
int ab = 0, bc = 0/*, ca = 0*/; // 同上

void* pthread1_func(void* arg){
    while(1){
    pthread_mutex_lock(&mutex);
    /* if(ca == 0) */
    /*     pthread_cond_wait(&cond_ca, &mutex); */
    printf("A->");
    ab++;
    pthread_cond_signal(&cond_ab);
    pthread_mutex_unlock(&mutex);

    pthread_mutex_lock(&mutex);
    /* if(bc == 0) // 一定得用while，因为条件变量会在没有任何人调用 signal 的情况下，莫名其妙把线程唤醒 */
    while(bc == 0)
        pthread_cond_wait(&cond_bc, &mutex);
    printf("C\n");
    bc--; 
    pthread_mutex_unlock(&mutex);

    sleep(1); // 为了观察效果
    }
    pthread_exit((void*)NULL);
}

void* pthread2_func(void* arg){
    while(1){
        pthread_mutex_lock(&mutex);
        /* if(ab == 0) */
        while(ab == 0)
            pthread_cond_wait(&cond_ab, &mutex);
        printf("B->");
        ab--;
        bc++;
        pthread_cond_signal(&cond_bc);
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit((void*)NULL);
}

int main(int argc, char* argv[]){                                  
    // 锁和条件变量的初始化一定要在创建线程前
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_ab, NULL);
    pthread_cond_init(&cond_bc, NULL);

    pthread_t pthread1_id, pthread2_id;
    pthread_create(&pthread1_id, NULL, pthread1_func, NULL);
    pthread_create(&pthread2_id, NULL, pthread2_func, NULL);

    pthread_join(pthread1_id, NULL);
    pthread_join(pthread2_id, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_ab);
    pthread_cond_destroy(&cond_bc);
    return 0;
}

