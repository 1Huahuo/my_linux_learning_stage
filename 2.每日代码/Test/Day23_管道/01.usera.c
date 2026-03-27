#include <my_header.h>

/*

*/
int main(int argc, char* argv[]){                                  
    int write_fd = open("1.pipe", O_WRONLY); int read_fd = open("2.pipe", O_RDONLY); ERROR_CHECK(write_fd, -1, "open 1.pipe"); ERROR_CHECK(read_fd, -1, "open 2.pipe"); while(1){ // 先让a给b发， a从键盘读取信息 printf("please input message:\n"); char buff[100] = {0}; ssize_t ret = read(STDIN_FILENO, buff, sizeof(buff));
        if(ret == 0) // 用户按下ctrl+d,终止标准输入
            break;
        // 将信息写到管道里去
        printf("stdin input count = %ld\n", ret);
        write(write_fd, buff, ret);
        
        // 从管道中读信息
        char buff2[100] = {0};
        read(read_fd, buff2, sizeof(buff2));
        printf("message from userb:%s\n", buff2);
    }

    close(write_fd);
    close(read_fd);
    return 0;
}

