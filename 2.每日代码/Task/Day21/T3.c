#include <my_header.h>

long long current_time_millis(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts); // 获取当前时间
    return (long long)ts.tv_sec * 1000LL + ts.tv_nsec / 1000000; // 转换为毫秒
}

/* 
open 有 二参 与 三参 两种形态
*/
int main(int argc, char *argv[]){
    ARGS_CHECK(argc, 3);
    int fdr = open(argv[1], O_RDONLY);
    ERROR_CHECK(fdr, -1, "open read");
    int fdw = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666); // !若有创建标志则一定要有文件权限
    ERROR_CHECK(fdw, -1, "open write");

    long long time1 = current_time_millis();
    char buff[100] = {0};
    ssize_t count = 0;
    while((count = read(fdr, buff, 100)) > 0) // != 0 严格来说不准确，没有考虑到读取失败的情况
        write(fdw, buff, count);
    long long time2 = current_time_millis();

    printf("缓冲字符数组大小为100时复制test.txt文件所需时间是：%lldms\n", time2 - time1);
    close(fdr);
    close(fdw);
    return 0;
}
