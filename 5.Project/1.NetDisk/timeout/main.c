#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "timewheel.h"
#include <my_header.h>

#define MAX_EVENTS 1024

// 设置文件描述符为非阻塞
static int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main() {
    // 创建时间轮: 61 个槽位，1 秒间隔，超时时间 60 秒
    TimeWheel *tw = time_wheel_create(11, 1);
    if (!tw) {
        fprintf(stderr, "Failed to create time wheel\n");
        return -1;
    }

    // 创建 epoll 实例
    int epfd = epoll_create1(0);
    if (epfd < 0) {
        perror("epoll_create1");
        time_wheel_destroy(tw);
        return -1;
    }

    // 创建监听 socket (示例: 监听 8080 端口)
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("socket");
        close(epfd);
        time_wheel_destroy(tw);
        return -1;
    }
    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("192.168.85.128");
    addr.sin_port = htons(8080);
    if (bind(listen_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(listen_fd);
        close(epfd);
        time_wheel_destroy(tw);
        return -1;
    }
    if (listen(listen_fd, 128) < 0) {
        perror("listen");
        close(listen_fd);
        close(epfd);
        time_wheel_destroy(tw);
        return -1;
    }
    set_nonblocking(listen_fd);
    struct epoll_event ev = {EPOLLIN, .data.fd = listen_fd};
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &ev) < 0) {
        perror("epoll_ctl");
        close(listen_fd);
        close(epfd);
        time_wheel_destroy(tw);
        return -1;
    }

    struct epoll_event events[10] = {0};
    printf("Server started on port 8080\n");

    while (1) {
        int nfds = epoll_wait(epfd, events, 10, 1000);
        time_t now = time(NULL);
        // 驱动时间轮，关闭超时连接
        int closed = time_wheel_tick(tw, now);
        if (closed > 0) {
            printf("Closed %d timeout connections\n", closed);
        }

        for (int i = 0; i < nfds; ++i) {
            int fd = events[i].data.fd;
            if (fd == listen_fd) {
                // 接受新连接
                int connfd = accept(listen_fd, NULL, NULL);
                if (connfd < 0) continue;
                set_nonblocking(connfd);
                time_wheel_add(tw, connfd);
                struct epoll_event ev_conn = {EPOLLIN | EPOLLET, .data.fd = connfd};
                epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev_conn);
                printf("New connection fd=%d added\n", connfd);
            } else {
                // 普通连接: 有数据可读，刷新超时计时
                time_wheel_refresh(tw, fd);
                // 这里可以读取数据并处理
                // 如果读取到 EOF 或错误，应主动关闭连接
                char buf[4096];
                int n = recv(fd, buf, sizeof(buf), 0);
                if (n <= 0) {
                    // 连接关闭或出错
                    if (n < 0 && errno != EAGAIN) {
                        perror("recv");
                    }
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                    time_wheel_del(tw, fd);
                    close(fd);
                    printf("Connection fd=%d closed\n", fd);
                } else {
                    // 收到数据，简单回显
                    send(fd, buf, n, 0);
                }
            }
        }
    }

    close(listen_fd);
    close(epfd);
    time_wheel_destroy(tw);
    return 0;
}
