#include <fcntl.h>
#include <my_header.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]){

    int fd = open("6.txt", O_WRONLY | O_CREAT | O_TRUNC, 0654);
    ERROR_CHECK(fd, -1, "open failed");

    ssize_t ret = write(fd, "hello", 4);
    // n 表示，这次实际写多少数据出去。 
    // read n 表示最多能装n个数据。
    //
    printf("ret = %ld\n", ret);

    // 一定要注意 read  write 的 n 的含义区别。 
    // read 的n.表明数组的长度，或者最多能装多少，实际装多少
    // 取决于，文件还剩多少；
    // write 的n 表明，数据要写出多少。

    close(fd);

    return 0;
}

