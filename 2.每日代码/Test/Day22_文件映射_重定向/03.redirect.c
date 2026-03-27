#include <my_header.h>

/*

*/
int main(int argc, char* argv[]){                                  
    ARGS_CHECK(argc, 2);
    printf("Hello world-------------------\n");
    int fd = open(argv[1], O_RDWR);
    ERROR_CHECK(fd, -1, "open");
    
    // !!!文件描述符的值不会改变，变得是与文件对象建立的连接
    int stdout_bak = 7; // 无特殊含义，随意一个整数值
    dup2(STDOUT_FILENO, stdout_bak); // stdout_bak=7--->STDOUT_FILENO=1--->STDOUT
    printf("111111111111111\n"); // 应该输出到终端

    dup2(fd, STDOUT_FILENO); // STDOUT_FILENO=1--->fd=3--->argv[1](即test.txt)
    printf("222222222222222\n"); // 应该输出到test.txt
    
    dup2(stdout_bak, STDOUT_FILENO); // STDOUT_FILENO=1--->stdout_back=7--->STDOUT
    printf("333333333333333\n"); // 应该输出到终端

    /* fflush(stdout); // FILE* stdout; */
    close(fd);
    close(stdout_bak);
    return 0;
}

