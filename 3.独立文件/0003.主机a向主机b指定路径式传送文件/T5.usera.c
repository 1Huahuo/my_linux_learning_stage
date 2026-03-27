#include <my_header.h>
#define BUF_SIZE 1024
/*
用法：./T5.usera 主机a上待发送文件的绝对路径 想发送到主机b上的绝对路径
举例：主机a上/home/a/test/1.txt这个文件我想发送到主机b的/home/b/file/2.txt这个地方，途中缺失的目录与文件会自动创建
	 那么完整的shell命令就是："./T5.usera /home/a/test/1.txt /home/b/file/2.txt"
*/
int main(int argc, char* argv[]){                                  
    ARGS_CHECK(argc, 3);

    char* file1_path = argv[1]; // 主机a上待发送的文件的路径
    char* file2_path = argv[2]; // 主机b上待接收的文件的路径

    int fd = open(file1_path, O_RDONLY);
    ERROR_CHECK(fd, -1, "open file");
    int fd_pipe = open("1.pipe", O_WRONLY);
    ERROR_CHECK(fd_pipe, -1, "open pipe");

    // 获取文件信息
    struct stat st;
    fstat(fd, &st);
    off_t file_size = st.st_size; // 待发送文件的大小
    // 发送路径长度
    int path_len = strlen(file2_path);
    write(fd_pipe, &path_len, sizeof(int));
    // 发送路径
    write(fd_pipe, file2_path, path_len);
    // 发送文件大小
    write(fd_pipe, &file_size, sizeof(off_t));
    // 发送文件内容
    char buf[BUF_SIZE];
    ssize_t count_read;
    while((count_read = read(fd, buf, BUF_SIZE)) > 0)
        write(fd_pipe, buf, count_read);

    close(fd);
    close(fd_pipe);
    return 0;
}
