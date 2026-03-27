#include <my_header.h>
#define BLOCK_SIZE (1024 * 1024 * 8) // 定义每次复制的块的大小为8M

long long current_time_millis() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts); // 获取当前时间
    return (long long)ts.tv_sec * 1000LL + ts.tv_nsec / 1000000; // 转换为毫秒
}

/*
利用mmap实现大文件复制的思路如下：
    分别open源文件和目标文件，并计算源文件大小
    ftruncate设置目标文件大小，避免文件大小不足而导致的映射失败也能够提前预留空间，
避免因空间不足而复制失败。
    记录文件复制时dest目标文件的偏移量offset，也就是已复制字节的数量。
利用该变量进行循环复制源文件到目标文件。
*/
int main(int argc, char* argv[]){                                  
    long t1 = current_time_millis();
    ARGS_CHECK(argc, 3);
   
    int src_fd = open(argv[1], O_RDONLY);
    ERROR_CHECK(src_fd, NULL, "open source txt");

    off_t src_size = lseek(src_fd, 0L, SEEK_END); // 这个不用检查，因为已经打开src.txt成功了
    // 另一种方法，即使用fstat
    /* struct stat src_stat = {0}; */
    /* int ret = fstat(src_fd, &src_stat); */
    /* ERROR_CHECK(ret, NULL, "fstat"); */
    /* off_t src_size = src_stat.st_size; */
    
    int dest_fd = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, 0666);
    ERROR_CHECK(dest_fd, NULL, "open destination txt");
    
    int ret = ftruncate(dest_fd, src_size);
    ERROR_CHECK(ret, -1, "ftruncate");

    off_t bytes_copied = 0;
    while(bytes_copied < src_size){
        off_t cur_size = bytes_copied + BLOCK_SIZE < src_size ? BLOCK_SIZE : src_size - bytes_copied;

        char* src_p = (char*)mmap(NULL, cur_size, PROT_READ | PROT_WRITE, MAP_SHARED, src_fd, bytes_copied);
        char* dest_p = (char*)mmap(NULL, cur_size, PROT_READ | PROT_WRITE, MAP_SHARED, dest_fd, bytes_copied);
        memcpy(dest_p, src_p, cur_size);

        munmap(src_p, cur_size);
        munmap(dest_p, cur_size);

        bytes_copied += cur_size;
    }

    close(src_fd);
    close(dest_fd);
    long t2 = current_time_millis();
    printf("用mmap复制一个大文件所需的时间 = %ld\n", (t2 - t1));
    return 0;
}
