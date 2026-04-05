#include <my_header.h>

int main(int argc, char* argv[]){                                  
    char* ip = "192.168.85.128";
    char* port = "12345";


    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(atoi(port));

    int client_fd = socket(AF_INET, SOCK_DGRAM, 0);

    fd_set set;

    while(1){
        FD_ZERO(&set);
        FD_SET(STDIN_FILENO, &set);
        FD_SET(client_fd, &set);

        select(client_fd + 1, &set, NULL, NULL, NULL);

        if(FD_ISSET(STDIN_FILENO, &set)){
            char buf[60] = {};
            read(STDIN_FILENO, buf, sizeof(buf));
            int ret_sendto = sendto(client_fd, buf, sizeof(buf), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
            ERROR_CHECK(ret_sendto, -1, "sendto");
        }
        if(FD_ISSET(client_fd, &set)){
            char buf[60] = {};
            struct sockaddr_in server_addr;
            socklen_t len = sizeof(server_addr);
            recvfrom(client_fd, buf, sizeof(buf), 0, (struct sockaddr*)&server_addr, &len);
            printf("from %s:%d %s\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port), buf);
        }
    }

    close(client_fd);
    return 0;
}

