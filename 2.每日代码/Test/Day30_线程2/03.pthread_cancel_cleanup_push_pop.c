#include <my_header.h>

void cleanheap(void *argp){
    printf("cleanheap is runing \n");
    free(argp);
}
void cleanfile(void *filep){
    printf("cleanfile is runing \n");
    int *fdP = (int *)filep;
    close(*fdP);
} 

void* func(void *arg){
    void *p1 = malloc(4);
    pthread_cleanup_push(cleanheap, p1);
    void *p2 = malloc(4);
    pthread_cleanup_push(cleanheap, p2);

    int fd = open("file1", O_RDWR); // ps： 注意，代码未必能执行到这个位置
    pthread_cleanup_push(cleanfile, &fd);

    // sleep(2);

    pthread_cleanup_pop(1);
    pthread_cleanup_pop(1);
    pthread_cleanup_pop(1);
    return NULL;
}

int main(int argc, char * argv[]){
    pthread_t  pid;
    int res = pthread_create(&pid, NULL, func, NULL);
    THREAD_ERROR_CHECK(res, "pthread_create");

    // sleep(1);
    int res_cancel = pthread_cancel(pid);
    THREAD_ERROR_CHECK(res_cancel, "pthread_cancel");

    int res_join = pthread_join(pid, NULL);
    THREAD_ERROR_CHECK(res_join, "pthread_join");

    return 0;
}
