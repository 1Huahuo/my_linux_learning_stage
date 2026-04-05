#include <my_header.h>

int main(int argc, char* argv[]){                                  
    char* server_ip = "192.168.85.128";
    char* server_port = "12345";

    // 创建服务器的网络套接字，并返回文件描述符
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(client_fd, -1, "socket");
    
    //设置端口复用
    int opt = 1;
    setsockopt(client_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    // 绑定服务器的IP地址和端口号
    struct sockaddr_in server_addr; // IPv4的地址结构
    memset(&server_addr, 0, sizeof(server_addr)); // 初始化
    server_addr.sin_family = AF_INET; // 地址的类型(IPv4协议族)
    server_addr.sin_addr.s_addr = inet_addr(server_ip); // 本机字节序转换为网络字节序
    server_addr.sin_port = htons(atoi(server_port));

    /* // bind：绑定端口和IP地址 */
    /* int ret_bind = bind(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)); */
    /* ERROR_CHECK(ret_bind, -1, "bind"); */

    /* // listen：监听 */
    /* int ret_listen = listen(client_fd, 10); */
    /* ERROR_CHECK(ret_listen, -1, "listen"); */
    /* printf("server is listening\n"); */

    /* // accept：获取连接,从全连接队列中取出一条连接，并且返回一个用于通信的文件描述符 */
    /* /1* int client_fd = accept(client_fd, NULL, NULL); // 不想知晓客户端的IP地址和端口号 *1/ */
    /* socklen_t len = sizeof(client_addr); */
    /* int client_fd = accept(client_fd, (struct sockaddr*)&client_addr, &len); */

    // 客户端只调用connect函数进行三次握手
    int ret_connect = connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    ERROR_CHECK(ret_connect, -1, "connect");

    // 定义读位图变量
    fd_set set;
    FD_ZERO(&set);

    while(1){
        FD_SET(STDIN_FILENO, &set);
        FD_SET(client_fd, &set);

        int ret_select = select(10, &set, NULL, NULL, NULL); // 后四个参数：读、写、错误、阻塞时长
        ERROR_CHECK(ret_select, -1, "select");
        printf("ret_select = %d\n", ret_select);

        // 客户端终端输入
        if(FD_ISSET(STDIN_FILENO, &set)) {
            // 通过键盘输入数据，然后存放在buf1中
            char buf1[50] = {};
            read(STDIN_FILENO, buf1, sizeof(buf1));
            // 需要将buf中的数据传输给对端(服务器)
            send(client_fd, buf1, sizeof(buf1), 0);
        }

        // 客户端接收到服务器发送的数据
        if(FD_ISSET(client_fd, &set)){
            char buf2[50] = {};
            int ret_recv = recv(client_fd, buf2, sizeof(buf2), 0);
            printf("ret_recv = %d\n", ret_recv);
            if(0 == ret_recv){
                printf("服务器断开连接\n");
                break;
            }
            printf("client recv from server: %s\n", buf2);
        }
    }

    close(client_fd);
    return 0;
}
