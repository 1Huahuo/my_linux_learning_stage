#include <my_header.h>

pthread_mutex_t mutex;
pthread_cond_t cond_ab, cond_ba;
int flag_ab = 0, flag_ba = 0;

void* pthreadA_func(void* arg){
    /* while(1){ */
    do{
        pthread_mutex_lock(&mutex);
        printf("Before A!\n");
        sleep(3);
        printf("After A\n");
        flag_ab++;
        pthread_cond_signal(&cond_ab);
        pthread_mutex_unlock(&mutex);

        pthread_mutex_lock(&mutex);
        while(flag_ba == 0)
            pthread_cond_wait(&cond_ba, &mutex);
        flag_ba--;
        pthread_mutex_unlock(&mutex);
    }while(0);
    pthread_exit((void*)NULL);
}

void* pthreadB_func(void* arg){
    /* while(1){ */
    do{
        pthread_mutex_lock(&mutex);
        while(flag_ab == 0)
            pthread_cond_wait(&cond_ab, &mutex);
        printf("Before B!\n");
        sleep(3);
        printf("After B\n");
        flag_ab--;
        flag_ba++;
        pthread_cond_signal(&cond_ba);
        pthread_mutex_unlock(&mutex);
    }while(0);
    pthread_exit((void*)NULL);
}

int main(int argc, char* argv[]){                                  
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_ab, NULL);
    pthread_cond_init(&cond_ba, NULL);

    pthread_t pthreadA_id, pthreadB_id;
    pthread_create(&pthreadA_id, NULL, pthreadA_func, NULL);
    pthread_create(&pthreadB_id, NULL, pthreadB_func, NULL);

    pthread_join(pthreadA_id, NULL);
    pthread_join(pthreadB_id, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_ab);
    pthread_cond_destroy(&cond_ba);
    return 0;
}

