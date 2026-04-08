#include <my_header.h>

int main(int argc, char* argv[]){                                  
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(client_fd, -1, "socket");

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("192.168.85.128");
    server_addr.sin_port = htons(atoi("12345"));

    int ret_connect = connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    ERROR_CHECK(ret_connect, -1, "connect");

    printf("选择想要获取的文件:输入0代表A.txt,输入1代表B.txt\n");
    int file_type;
    /* read(STDIN_FILENO, &file_type, sizeof(file_type)); */
    fscanf(stdin, "%d", &file_type);
    printf("file_type = %d\n", file_type);
    send(client_fd, &file_type, sizeof(file_type), MSG_NOSIGNAL);

    int filename_len = 0;
    recv(client_fd, &filename_len, sizeof(filename_len), MSG_WAITALL);

    char file_name[60] = {};
    recv(client_fd, file_name, filename_len, MSG_WAITALL);

    off_t file_size = 0;
    recv(client_fd, &file_size, sizeof(file_size), MSG_WAITALL);
    printf("file size = %ld\n", file_size);

    int file_fd = open(file_name, O_RDWR | O_CREAT | O_TRUNC, 0600);
    ftruncate(file_fd, file_size);

    // 内存映射，第一个参数是映射到内存的基地址，设为NULL代表由系统自动分配
    char* p = (char*)mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, file_fd, 0);
    recv(client_fd, p, file_size, MSG_WAITALL);
    munmap(p, file_size);

    close(file_fd);
    close(client_fd);
    return 0;
}

