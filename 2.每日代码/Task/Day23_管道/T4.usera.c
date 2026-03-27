#include <my_header.h>

/*

*/
int main(int argc, char* argv[]){                                  
    int fd = open("3.pipe", O_WRONLY);

    char buff[1024];
    int length;
    ssize_t read_stdin;
    // 写文件名长度与文件名进管道
    printf("请依次写入文件的名字、长度、内容：\n");
    
    read_stdin = read(STDIN_FILENO, buff, sizeof(buff));
    write(fd, buff, read_stdin - 1); // 去掉换行符
    sleep(5);

    scanf("%d", &length);
    write(fd, &length, sizeof(int)); // 去掉换行符
    sleep(5);

    read_stdin = read(STDIN_FILENO, buff, sizeof(buff));
    write(fd, buff, read_stdin - 1);

    return 0;
}
