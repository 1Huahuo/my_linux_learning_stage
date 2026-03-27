#include <my_header.h>

/*
匿名管道/pipe函数是Linux系统中用于实现父子进程间通信的一种简单方式
*/
int main(int argc, char* argv[]){                                  
    int pipefd[2];
    pipe(pipefd);

    if(fork() == 0){
        close(pipefd[1]);
        char buf[1024];
        read(pipefd[0], buf, 1024);
        printf("child get message:%s\n", buf);
    }else{
        close(pipefd[0]);
        write(pipefd[1], "hello", 6);
        wait(NULL);
    }

    return 0;
}

