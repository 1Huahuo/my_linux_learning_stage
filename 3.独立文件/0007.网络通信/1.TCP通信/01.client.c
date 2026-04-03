#include <my_header.h>

int main(int argc, char* argv[]){                                  

    char* server_ip = "192.168.85.128";
    char* server_port = "12345";

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

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

    int ret_connect = connect(socket_fd, (struct sockaddr*)&socket_addr, sizeof(socket_addr));
    ERROR_CHECK(ret_connect, -1, "connect");

    /* while(1){ */
        char buf1[1024] = {};

        // 读标准输入
        read(STDIN_FILENO, buf1, sizeof(buf1));

        // 把标准输入发给服务器
        int ret_send = send(socket_fd, buf1, sizeof(buf1), 0);
        ERROR_CHECK(ret_send, -1, "send");

        char buf2[1024] = {};
        // 读取对方输入
        int ret_recv = recv(socket_fd, buf2, sizeof(buf2), 0);
        ERROR_CHECK(ret_recv, -1, "recv");
        ERROR_CHECK(ret_recv, 0, "other close");

        // 打印到标准输出
        write(STDOUT_FILENO, buf2, sizeof(buf2));
    /* } */

    close(socket_fd);
    return 0;
}
