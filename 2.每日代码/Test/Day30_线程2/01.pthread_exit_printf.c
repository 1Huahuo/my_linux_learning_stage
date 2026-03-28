#include <my_header.h>

void *funThread(void *arg)
{
    printf("i am child thread"); // 这句话并未立即输出,而是等了一会 才输出
                                 // 退出线程的参数要求是void*类型 or NULL
    pthread_exit((void *) 11);
    printf("this scene well not be print \n");
}
int main(int argc, char * argv[])
{
    pthread_t threadId;
    int ret = pthread_create(&threadId, NULL, funThread, NULL);
    THREAD_ERROR_CHECK(ret, "pthread_create");
    sleep(5);
    printf("i am main thread \n");
    return 0;
}
