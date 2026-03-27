#include <my_header.h>

//void test(){
//    ARGS_CHECK(argc, 2);
//    int fd = open(argv[1], O_RDONLY);
//    ERROR_CHECK(fd, -1, "open failed");
//
//    char buf[10] = {0};
//    ssize_t ret = read(fd, buf, sizeof(buf) - 1);
//    printf("ret = %ld\n", ret);
//    printf("%s-----\n", buf);
//
//    ssize_t ret2 = read(fd, buf, sizeof(buf) - 1); 
//    printf("ret2 = %ld\n", ret2);
//    printf("%s-----\n", buf);
//}

int main(int argc, char *argv[]){
    ARGS_CHECK(argc, 2);
    int fd = open(argv[1], O_RDONLY);
    ERROR_CHECK(fd, -1, "open failed");

    char buf[10] = {0};
    ssize_t ret = read(fd, buf, sizeof(buf));
    printf("ret = %ld\n", ret);
    printf("%s-----\n", buf);
    // read 不会填充任何额外的数据 而是里面有什么数据就装入什么数据
    // 不会和其他的函数一样 类似于 fgets会在末尾添'\0'，注：fgetc与fread则不会
    // ret即read的返回值读取成功时是此次读取到的字节个数,若是读完即返回0也即最后一次读取0个字节


    close(fd);

    return 0;
}

