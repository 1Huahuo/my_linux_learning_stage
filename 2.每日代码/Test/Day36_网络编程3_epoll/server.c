#include <my_header.h>

int main(int argc, char* argv[]){                                  
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    char* ip = "192.168.85.128";
    char* port = "12345";

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(atoi(port));

    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));       

    listen(server_fd, 100); // 监听请求
    
    int client_fd = accept(server_fd, NULL, 0);

    // 设计监听集合
    int epoll_fd = epoll_create(1);

    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = client_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event);
    event.data.fd = STDIN_FILENO;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &event);

    while(1){
        struct epoll_event events[2];
        int ready_count = epoll_wait(epoll_fd, events, 2, 0);

        for(int i = 0; i < ready_count; i++){
            int fd = events[i].data.fd;
            if(fd == STDIN_FILENO){
                char buf[60] = {};
                read(STDIN_FILENO, buf, sizeof(buf));
                send(client_fd, buf, sizeof(buf), 0);
            }else if(fd == client_fd){
                char buf[60] = {};
                int ret_recv = recv(client_fd, buf, sizeof(buf), 0);
                if(0 == ret_recv){
                    printf("客户端断开\n");
                    goto end;
                }
                write(STDOUT_FILENO, buf, sizeof(buf));
            }
        }
    }

end:
    close(client_fd);
    close(server_fd);
    return 0;
}

