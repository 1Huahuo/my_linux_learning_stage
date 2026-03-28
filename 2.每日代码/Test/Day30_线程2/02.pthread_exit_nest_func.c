#include <my_header.h>

void funThreadDeep(){
    printf("this is deep function\n");
    // 嵌套调用函数时调用退出线程函数，依旧导致所在线程退出
    // 即使当前方法不是线程的入口函数
    pthread_exit(NULL);
    printf("this scene2 well not be print\n");
}

void* funThread(void* arg){
    funThreadDeep(); // 调用另一个方法
    printf("this scene well not be print\n");
    return NULL;
}

int main(int argc, char* argv[]){                                  
    pthread_t tid;
    int ret = pthread_create(&tid, NULL, funThread, NULL);
    THREAD_ERROR_CHECK(ret, "pthread_create");
    sleep(10);

    return 0;
}

