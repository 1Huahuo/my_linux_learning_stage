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

    off_t size = lseek(fd, 0L, SEEK_END);
    char* p = (char*)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    char* t = p;
    /* while(*t != EOF){ // 注意：拿的是一个确定大小的内存块不会拿到文件终止符！与fgetc不同 */
    int count = size;
    while(count--){
        if(*t >= 'A' && *t <= 'Z')
            *t = *t + 'a' - 'A';
        t++;
    }

    munmap(p, size);
    close(fd);
    long t2 = current_time_millis();
    printf("用mmap实现的tolower处理一个文件所需的时间 = %ldms\n", (t2 - t1));
    return 0;
}

