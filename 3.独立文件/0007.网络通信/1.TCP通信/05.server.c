#include <my_header.h>

int main(int argc, char* argv[]){                                  
    char* server_ip = "192.168.85.128";
    char* server_port = "12345";

    // 创建服务器的网络套接字，并返回文件描述符
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(server_fd, -1, "socket");

    //设置端口复用
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    // 绑定服务器的IP地址和端口号
    struct sockaddr_in server_addr; // IPv4的地址结构
    memset(&server_addr, 0, sizeof(server_addr)); // 初始化
    server_addr.sin_family = AF_INET; // 地址的类型(IPv4协议族)
    server_addr.sin_addr.s_addr = inet_addr(server_ip); // 本机字节序转换为网络字节序
    server_addr.sin_port = htons(atoi(server_port));

    // bind：绑定端口和IP地址
    int ret_bind = bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    ERROR_CHECK(ret_bind, -1, "bind");

    // listen：监听
    int ret_listen = listen(server_fd, 10);
    ERROR_CHECK(ret_listen, -1, "listen");
    printf("server is listening\n");

    // 定义读位图变量
    fd_set set;
    FD_ZERO(&set); 
    // 初始只需要监听server_fd，只要其读就绪就代表全连接队列可读，即意味着可以取客户端连接
    FD_SET(server_fd, &set);

    int arr[1024]; // 可以同时储存多个连接中的客户端的文件描述符
    memset(arr, 0, sizeof(arr));

    int client_fd = -1; // 在while外面创建，相当于全局变量，不然只能在取出连接的循环中有效
    while(1){
        fd_set tmp; // 创建一个副本，使得set只负责存储而不被修改，不负责监听
        tmp = set; // 临时位图，专门负责监听

        int ret_select = select(10, &tmp, NULL, NULL, NULL); // 后四个参数：读、写、错误、阻塞时长
        ERROR_CHECK(ret_select, -1, "select");
        printf("ret_select = %d\n", ret_select);

        // server_fd就绪，即从全连接队列中取出一个客户端连接，即调用accept
        if(FD_ISSET(server_fd, &tmp)){
            printf("取出一条客户端连接\n");

            // 取出新连接时将旧连接移出监听集合，解决死循环问题
            /* if(-1 != client_fd) */
            /*     FD_CLR(client_fd, &set); */

            client_fd = accept(server_fd, NULL, NULL); // while外创建，此处初始化
            ERROR_CHECK(client_fd, -1, "accept");
            // 加入专门用来监听客户端的临时位图tmp
            FD_SET(client_fd, &set); // 此时加入的就是下方的最新arr数组中的非0元素
            FD_SET(STDIN_FILENO, &set); // 同时开始监听标准输入文件描述符
            /* FD_CLR(server_fd, &set); // 不想继续取出其他客户端连接 */

            for(int idx = 0; idx < 1024; idx++)
                if(0 == arr[idx]){
                    arr[idx] = client_fd;
                    break;
                }
        }

        for(int idx = 0; idx < 1024; idx++){
            if(0 == arr[idx])
                continue;

            // 服务器终端输入
            if(FD_ISSET(STDIN_FILENO, &tmp)) {
                // 通过键盘输入数据，然后存放在buf1中
                char buf1[50] = {};
                read(STDIN_FILENO, buf1, sizeof(buf1));
                // 需要将buf中的数据传输给对端(客户端)
                send(arr[idx], buf1, sizeof(buf1), 0);
            }

            // 服务器接收到客户端发送的数据
            if(FD_ISSET(arr[idx], &tmp)){
                char buf2[50] = {};
                int ret_recv = recv(arr[idx], buf2, sizeof(buf2), 0);
                printf("ret_recv = %d\n", ret_recv);
                if(0 == ret_recv){
                    printf("一个客户端断开连接\n");
                    arr[idx] = 0; // 使代码更加健壮，使得arr数组可以复用
                    FD_CLR(arr[idx], &set); // 不再监听断开客户端的连接
                    FD_CLR(STDIN_FILENO, &set); // 假设没有连接了，则停止监听终端输入从而避免发送给断开的连接
                    /* FD_SET(server_fd, &set); // 同时只能取出一个连接时使用 */
                    close(arr[idx]); // 关闭断开客户端的文件描述符
                    continue; // 直接继续下一次循环
                }
                printf("server recv from client: %s\n", buf2);
            }
        }
    }

    close(server_fd);
    close(client_fd);
    return 0;
}
