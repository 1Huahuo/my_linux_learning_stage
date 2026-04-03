#include <my_header.h>

int main(int argc, char* argv[]){                                  
    char* server_ip = "192.168.85.128";
    char* server_port = "12345";

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    //设置端口重用
    int opt = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    //in_addr_t addrTIP = inet_addr(sourceIP);
    //struct in_addr inAddr;
    //inAddr.s_addr = addrTIP;
    struct in_addr server_inet_addr;
    inet_aton(server_ip, &server_inet_addr); // IP地址转换成网络字节序

    int server_int_port = atoi(server_port); // 端口号转换为int类型
    int server_inet_port = htons(server_int_port); // 端口号转换为网络字节序

    struct sockaddr_in socket_addr;
    socket_addr.sin_family = AF_INET;
    socket_addr.sin_addr = server_inet_addr;
    socket_addr.sin_port = server_inet_port;

    // bind：绑定端口和IP地址
    int ret_bind = bind(socket_fd, (struct sockaddr*)&socket_addr, sizeof(socket_addr));
    ERROR_CHECK(ret_bind, -1, "bind");

    // listen：监听
    listen(socket_fd, 10);

    // accept：获取连接
    int connect_fd = accept(socket_fd, NULL, NULL);

    /* while(1){ */
        char buf1[10] = {};
        // 读取对方输入
        int ret_recv = recv(connect_fd, buf1, sizeof(buf1), 0);
        ERROR_CHECK(ret_recv, -1, "recv");
        ERROR_CHECK(ret_recv, 0, "other close");

        // 打印到标准输出
        write(STDOUT_FILENO, buf1, sizeof(buf1));

        char buf2[10] = {};
        // 读取标准输入
        read(STDIN_FILENO, buf2, sizeof(buf2));

        // 把标准输入，发送给对方
        int ret_send = send(connect_fd, buf2, sizeof(buf2), 0);
        ERROR_CHECK(ret_send, -1, "send");
    /* } */

    close(socket_fd);
    return 0;
}
