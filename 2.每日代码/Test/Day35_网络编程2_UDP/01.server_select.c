#include <my_header.h>

int main(int argc, char* argv[]){                                  
    char* ip = "192.168.85.128";
    char* port = "12345";

    // 创建文件描述符
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(socket_fd, -1, "socket");

    // 设置端口复用
    int opt = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    // 绑定服务器的ip和端口号
    struct sockaddr_in server_addr, client_addr;

    memset(&server_addr, 0, sizeof(server_addr)); // 初始化server_addr
    
    server_addr.sin_family = AF_INET; // 地址的类型(IPv4协议族)
    server_addr.sin_addr.s_addr = inet_addr(ip); // 本机字节序转换为网络字节序
    server_addr.sin_port = htons(atoi(port));

    int ret_bind = bind(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    ERROR_CHECK(ret_bind, -1, "bind");

    // 监听
    int ret_listen = listen(socket_fd, 10);
    ERROR_CHECK(ret_listen, -1, "listen");
    printf("server is listening\n");

    // 从全连接队列中取出一条连接，并且返回一个用于通信的文件描述符
    /* int client_fd = accept(socket_fd, NULL, NULL); // 不想知晓客户端的ip与端口号 */
    socklen_t len = sizeof(client_addr);
    int client_fd = accept(socket_fd, (struct sockaddr*)&client_addr, &len);
    ERROR_CHECK(client_fd, -1, "accept");

    // 定义读位图变量
    fd_set set;
    FD_ZERO(&set);

    while(1){
        // 将标准输入与客户端的文件描述符添加到读位图中
        FD_SET(socket_fd, &set);
        FD_SET(client_fd, &set);

        int ret_select = select(10, &set, NULL, NULL, NULL); // 后四个参数：读、写、错误、阻塞时长
        ERROR_CHECK(ret_select, -1, "select");
        printf("ret_select = %d\n", ret_select);

        // 服务器终端输入
        if(FD_ISSET(STDIN_FILENO, &set)){
            // 通过键盘输入数据，然后存放在buf中
            char buf[50] = {};
            read(STDIN_FILENO, buf, sizeof(buf));
            // 将buf中数据传输给客户端(对端)
            send(client_fd, buf, sizeof(buf), 0);
        }
        // 服务器接收到客户端发送数据
        if(FD_ISSET(client_fd, &set)){
            char buf[50] = {};
            int ret_recv = recv(client_fd, buf, sizeof(buf), 0);
            printf("ret_recv = %d\n", ret_recv);
            if(0 == ret_recv){
                printf("客户端已关闭\n");
                break;
            }
            printf("server recv from client: %s\n", buf);
        }
    }

    close(socket_fd);
    close(client_fd);
    return 0;
}
