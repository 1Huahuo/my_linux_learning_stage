#include <my_header.h>

/* Usage:  */
int main(int argc, char *argv[]){                                  
    ARGS_CHECK(argc, 2);
    int fd = open(argv[1], O_RDONLY);
    ERROR_CHECK(fd, -1, "open");

    __off_t file_size = lseek(fd, 0L, SEEK_END);
    char *buff = (char*)malloc(file_size + 1);
    lseek(fd, 0L, SEEK_SET);
    ssize_t count = read(fd, buff, file_size);
    ERROR_CHECK(count, file_size, "read");

    close(fd);
    return 0;
}
