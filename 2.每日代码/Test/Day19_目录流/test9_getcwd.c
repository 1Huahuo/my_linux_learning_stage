#include <my_header.h>
#include <stdio.h>

int main(int argc, char *argv[]){

    /* char buf[100] = {0}; */
    char buf[10] = {0};
    char *ret = getcwd(buf, sizeof(buf));

    // 返回值，正常情况下就是buf字节这个指针。
    // 不正常情况为NULL。表明获取失败。

    printf("buf = %s\n", buf);
    printf("ret = %s\n", ret);
    printf("(buf == ret) = %d\n", (buf == ret));


    return 0;
}

