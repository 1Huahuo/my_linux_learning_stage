#include <my_header.h>
#include "send_file.h"

void send_file(int fd){
    /* send(fd, "hello", 5, 0); */
    char* file_name = "test.txt";
    int file_fd = open(file_name, O_RDWR);

    // 获取文件大小
    struct stat st;
    fstat(file_fd, &st);
    off_t file_size = st.st_size;

    int filename_len = strlen(file_name);
    send(fd, &filename_len, sizeof(filename_len), MSG_NOSIGNAL); // 注意这里的fd一定得是传参的那个即对端的文件描述符
    // 向已关闭的 TCP 连接写数据时，不触发 SIGPIPE 信号（防止程序意外退出），只返回错误码 EPIPE。
    send(fd, file_name, filename_len, MSG_NOSIGNAL);

    send(fd, &file_size, sizeof(file_size), MSG_NOSIGNAL);
    sendfile(fd, file_fd, NULL, file_size); // 零拷贝操作,这里的offset字段一定不能填0，因为0是一个野指针，可能会导致程序崩溃

    close(file_fd);
}
