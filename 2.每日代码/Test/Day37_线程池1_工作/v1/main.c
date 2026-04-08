#include <my_header.h>
#include "thread_pool.h"
#include "socket.h"
#include "epoll.h"

int main(int argc, char* argv[]){                                  
    thread_pool_t pool;
    init_thread_pool(&pool, 4);

    int server_fd = 0;
    init_socket("192.168.85.128", "12345", &server_fd);

    int epoll_fd = epoll_create(1);
    ERROR_CHECK(epoll_fd, -1, "epoll_create");

    add_epoll_fd(epoll_fd, server_fd);

    while(1){
        struct epoll_event events[10];
        int ready_count = epoll_wait(epoll_fd, events, 10, -1);
        ERROR_CHECK(ready_count, -1, "epoll_wait");
        printf("ready count = %d\n", ready_count);

        for(int idx = 0; idx < ready_count; idx++){
            int fd = events[idx].data.fd;
            if(fd == server_fd){
                int client_fd = accept(server_fd, NULL, NULL);
                ERROR_CHECK(client_fd, -1, "accept");

                pthread_mutex_lock(&pool.mutex);

                enQueue(&pool.queue, client_fd);
                pthread_cond_signal(&pool.cond);

                pthread_mutex_unlock(&pool.mutex);
            }
        }
    }

    return 0;
}

