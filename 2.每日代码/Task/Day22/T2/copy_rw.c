#include <my_header.h>
#define ONCE_READ_SIZE 4096

long long current_time_millis() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts); // 获取当前时间
    return (long long)ts.tv_sec * 1000LL + ts.tv_nsec / 1000000; // 转换为毫秒
}

/*

*/
int main(int argc, char* argv[]){                                  
    long t1 = current_time_millis();
    ARGS_CHECK(argc, 3);

    int src_fd = open(argv[1], O_RDONLY);
    ERROR_CHECK(src_fd, -1, "open src");
    int dest_fd = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, 0666);
    ERROR_CHECK(dest_fd, -1, "open dest");
    
    off_t src_size = lseek(src_fd, 0L, SEEK_END);
    lseek(src_fd, 0L, SEEK_SET); // read/write严格依赖于fd的偏移，所以一定要倒带
    int ret = ftruncate(dest_fd, src_size);
    ERROR_CHECK(ret, -1, "ftruncate");

    char buff[ONCE_READ_SIZE] = {0}; 
    ssize_t count = 0;
    while((count = read(src_fd, buff, ONCE_READ_SIZE)) != 0)
        write(dest_fd, buff, count);
    
    close(src_fd);
    close(dest_fd);

    long t2 = current_time_millis();
    printf("用read&write复制一个大文件所需的时间 = %ldms\n", (t2 - t1));
    return 0;
}
