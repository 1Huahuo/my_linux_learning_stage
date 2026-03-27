#include <my_header.h>

int retnum2;

void* func1(void* arg){
    return (void*)1;
}

void* func2(void* arg){
    retnum2 = 2;
    return (void*)&retnum2;
}

void* func3(void* arg){
    int* retnum3 = (int*)malloc(sizeof(int));
    *retnum3 = 3;

    return (int*)retnum3;
}

int main(int argc, char* argv[]){                                  
    pthread_t tid1, tid2, tid3;
    int ret_create1 = pthread_create(&tid1, NULL, func1, NULL);
    int ret_create2 = pthread_create(&tid2, NULL, func2, NULL);
    int ret_create3 = pthread_create(&tid3, NULL, func3, NULL);
    THREAD_ERROR_CHECK(ret_create1, "create thread1");
    THREAD_ERROR_CHECK(ret_create2, "create thread2");
    THREAD_ERROR_CHECK(ret_create3, "create thread3");

    void* retval1, *retval2, *retval3;
    int ret_join1 = pthread_join(tid1, &retval1);
    int ret_join2 = pthread_join(tid2, &retval2);
    int ret_join3 = pthread_join(tid3, &retval3);
    THREAD_ERROR_CHECK(ret_join1, "join thread1");
    THREAD_ERROR_CHECK(ret_join2, "join thread2");
    THREAD_ERROR_CHECK(ret_join3, "join thread3");
    
    printf("thread1 retval = %ld\n", (long)retval1);
    printf("thread2 retval = %d\n", *(int*)retval2);
    printf("thread3 retval = %d\n", *(int*)retval3);

    free(retval3);
    return 0;
}

