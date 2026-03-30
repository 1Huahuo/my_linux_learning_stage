#include <my_header.h>

int n = 0;

void* pthread_func(void* arg){
    for(int i = 0; i < 20000000; i++)
        n++;
    pthread_exit((void*)NULL);
}

int main(int argc, char* argv[]){                                  
    struct timeval time1, time2;
    gettimeofday(&time1, NULL);

    pthread_t pthread_id;
    pthread_create(&pthread_id, NULL, pthread_func, NULL);
    
    for(int i = 0; i < 20000000; i++)
        n++;
    pthread_join(pthread_id, NULL);

    gettimeofday(&time2, NULL);
    printf("不加锁时，消耗的时间：%ldus\n", 1000000 * (time2.tv_sec - time1.tv_sec) + time2.tv_usec - time1.tv_usec);
    return 0;
}

