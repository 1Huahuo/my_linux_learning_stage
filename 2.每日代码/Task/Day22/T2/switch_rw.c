#include <my_header.h>

long long current_time_millis() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts); // 获取当前时间
    return (long long)ts.tv_sec * 1000LL + ts.tv_nsec / 1000000; // 转换为毫秒
}

/*
   将文件中的全部大写字母转换成对应的小写字母
   */
int main(int argc, char* argv[]){                                  
    long t1 = current_time_millis();
    ARGS_CHECK(argc, 2);
    int fd = open(argv[1], O_RDWR);
    ERROR_CHECK(fd, -1, "open");

    char c = 0;
    while(1){
        ssize_t count = read(fd, &c, 1);
        if(count == 0)
            break;
        if(c >= 'A' && c <= 'Z'){
            c = c + 'a' - 'A';
            lseek(fd, -1L, SEEK_CUR); // read与write同时强依赖于fd的偏移，所以要移动fd才能保证操作正确
            write(fd, &c, 1);
        }

    }

    close(fd);
    long t2 = current_time_millis();
    printf("用read&write实现的tolower处理一个文件所需的时间 = %ldms\n", (t2 - t1));
    return 0;
}

