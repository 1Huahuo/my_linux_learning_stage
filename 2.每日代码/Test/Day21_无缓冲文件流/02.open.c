#include <my_header.h>

// 带创建标志时需要使用三参数open，并且传入第三个参数:文件权限
void test(){
    /* int fd = open("1.txt", O_WRONLY | O_CREAT); // 要加权限 */
    int fd = open("1.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666); 
    
    // 如果加了O_CREAT。 一定要给权限，否则就是随机权限。 
    // 如果只写一个O_WRONLY，且不存在此文件，则打开失败 
    ERROR_CHECK(fd, -1, "open failed");
}

int main(int argc, char *argv[]){
    // 之前打开有缓冲区文件流。使用fopen  现在使用open开无缓冲区文件流。
    printf("%o\n", O_RDONLY);
    printf("%o\n", O_WRONLY);
    printf("%o\n", O_RDWR);
    printf("%o\n", O_APPEND);

    int fd = open("1.txt", O_RDONLY);
    printf("fd = %d\n", fd);
    ERROR_CHECK(fd, -1, "open failed"); // -1代表打开失败，0代表读到EOF，正整数代表本次读取字节数

    return 0;
}

