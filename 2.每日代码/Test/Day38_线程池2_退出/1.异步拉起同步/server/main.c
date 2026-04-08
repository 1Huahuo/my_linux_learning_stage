#include <my_header.h>
#include "thread_pool.h"
#include "socket.h"
#include "epoll.h"

int pipe_fd[2];

void func(int num){
    // 信号触发
    printf("signal num = %d\n", num);
    write(pipe_fd[1], "1", 1);
    return;
}

int main(int argc, char* argv[]){                                  
    pipe(pipe_fd);

    if(fork() != 0){ // 父进程
        signal(2, func); // 注册2号信号，即键盘中断信号:Ctrl + C
        wait(NULL);
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        exit(0);
    }
    // 子进程
    setpgid(0, 0); // 键盘中断信号（SIGINT）只会发送给前台进程组中的所有进程，为避免子进程也会收到键盘中断信号，应让子进程处于另一个进程组，即处于后台进程组

    close(pipe_fd[1]); // 子进程中只保留对匿名管道的读

    thread_pool_t pool;
    init_thread_pool(&pool, 4);

    int server_fd = 0;
    init_socket("192.168.85.128", "12345", &server_fd);

    int epoll_fd = epoll_create(1);
    ERROR_CHECK(epoll_fd, -1, "epoll_create");
    add_epoll_fd(epoll_fd, server_fd);

    add_epoll_fd(epoll_fd, pipe_fd[0]); // 监听管道

    while(1){
        struct epoll_event events[10];
        int ready_count = epoll_wait(epoll_fd, events, 10, -1);
        ERROR_CHECK(ready_count, -1, "epoll_wait");
        printf("ready count = %d\n", ready_count);

        for(int idx = 0; idx < ready_count; idx++){
            int fd = events[idx].data.fd;

            if(fd == pipe_fd[0]){
                char buf[1];
                read(fd, buf, sizeof(buf));
                printf("子进程的主线程收到了父进程的结束信号\n");
                for(int idx = 0; idx < pool.thread_num; idx++)
                    pthread_cancel(pool.thread_id_arr[idx]); // 子进程中的主线程终止子线程
                for(int idx = 0; idx < pool.thread_num; idx++)
                    pthread_join(pool.thread_id_arr[idx], NULL); // 等待子线程结束
                pthread_exit(NULL); // 主线程主动退出
            }
            else if(fd == server_fd){
                int client_fd = accept(server_fd, NULL, NULL);
                ERROR_CHECK(client_fd, -1, "accept");

                pthread_mutex_lock(&pool.mutex);

                enQueue(&pool.queue, client_fd); // 新客户端fd入队
                pthread_cond_signal(&pool.cond);

                pthread_mutex_unlock(&pool.mutex);
            }
        }
    }

    return 0;
}
