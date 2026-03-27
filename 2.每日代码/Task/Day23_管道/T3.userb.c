#include <my_header.h>

/*

*/
int main(int argc, char* argv[]){                                  
    int read_fd = open("1.pipe", O_RDONLY);
    int write_fd = open("2.pipe", O_WRONLY);
    ERROR_CHECK(write_fd, -1, "open 1.pipe");
    ERROR_CHECK(read_fd, -1, "open 2.pipe");

    char buff[60];
    fd_set set; 

    while(1){
        FD_ZERO(&set);
        FD_SET(STDIN_FILENO, &set); // 监听标准输入流，为了写
        FD_SET(read_fd, &set); // 监听管道读, 为了读

        int res_select = select(read_fd + 1, &set, NULL, NULL, NULL); // 最后一个NULL代表阻塞到一个就绪为止
        if(FD_ISSET(read_fd, &set)){
            bzero(buff, sizeof(buff)); // 置零
            int read_count = read(read_fd, buff, sizeof(buff));
            if(read_count == 0){
                printf("user-a断开连接\n");
                break;
            }
            printf("user-a:%s", buff);
        }
        if(FD_ISSET(STDIN_FILENO, &set)){
            bzero(buff, sizeof(buff));
            int read_stdin = read(STDIN_FILENO, buff, sizeof(buff));
            if(read_stdin == 0) // 用户按下了Ctrl+D，输入了EOF，代表要终止标准输入
                break;
            write(write_fd, buff, read_stdin);
        }
    }

    close(write_fd);
    close(read_fd);
    return 0;
}

