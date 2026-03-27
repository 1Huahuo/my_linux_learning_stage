#include <my_header.h>

/*

*/
int main(int argc, char* argv[]){                                  
    int read_fd = open("1.pipe", O_RDONLY);
    int write_fd = open("2.pipe", O_WRONLY);
    ERROR_CHECK(write_fd, -1, "open 1.pipe");
    ERROR_CHECK(read_fd, -1, "open 2.pipe");

    while(1){
        // 先从管道中读信息
        char buff[100] = {0};
        read(read_fd, buff, sizeof(buff));
        printf("message from usera:%s\n", buff);
        
        // 再让b给a发， b从键盘读取信息
        printf("please input message:\n");
        char buff2[100] = {0};
        ssize_t ret = read(STDIN_FILENO, buff2, sizeof(buff2));
        if(ret == 0) // 用户按下ctrl+d;终止标准输入
            break;
        // 再将信息写到管道里去
        printf("stdin input count = %ld\n", ret);
        write(write_fd, buff2, ret);
    }

    close(write_fd);
    close(read_fd);
    return 0;
}

