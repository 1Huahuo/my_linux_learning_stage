#include <my_header.h>

int main(int argc, char* argv[]){                                  
    char* ip = "192.168.85.128";
    char* port = "12345";

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(atoi(port));

    int server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    fd_set set;

    while(1){
        FD_ZERO(&set);
        FD_SET(STDIN_FILENO, &set);
        FD_SET(server_fd, &set);
    
        select(10, &set, NULL, NULL, NULL);

        if(FD_ISSET(STDIN_FILENO, &set)){
            char buf[60] = {};
            read(STDIN_FILENO, buf, sizeof(buf));
            if(!client_addr.sin_port)
                printf("暂时未知客户端，等待客户端输入\n");
            else{
                int ret_sendto = sendto(server_fd, buf, sizeof(buf), 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
                printf("ret_sendto = %d\n", ret_sendto);
            }
        }
        if(FD_ISSET(server_fd, &set)){
            char buf[60] = {};
            socklen_t len = sizeof(client_addr);
            recvfrom(server_fd, buf, sizeof(buf), 0, (struct sockaddr*)&client_addr, &len);
            printf("from %s:%d : %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buf);
        }

    }

    close(server_fd);
    return 0;
}

