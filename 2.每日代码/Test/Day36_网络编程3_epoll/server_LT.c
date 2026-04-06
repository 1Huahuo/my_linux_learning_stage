#include <my_header.h>

int main(int argc, char* argv[]){                                  
    int epoll_fd = epoll_create(1);

    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = STDIN_FILENO;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &event);
    
    while(1){
        struct epoll_event events;
        events.data.fd = -1;
        epoll_wait(epoll_fd, &events, 1, 0);
        int fd = events.data.fd;

        if(fd == STDIN_FILENO){
            char buf[3] = {};
            int ret_read = read(STDIN_FILENO, buf, sizeof(buf));
            if(0 == ret_read){
                printf("输入为空，终止进程");
                break;
            }else{
                write(STDOUT_FILENO, buf, sizeof(buf));
                printf("---此输出无换行---");
                fflush_unlocked(stdout);
            }
        }
    }

    close(epoll_fd);
    return 0;
}
