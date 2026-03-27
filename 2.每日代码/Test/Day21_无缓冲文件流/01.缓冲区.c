#include <my_header.h>

int main(int argc, char *argv[]){
    FILE *fp = fopen("1.txt", "r");
    ERROR_CHECK(fp, NULL, "fopen failed");
    // FILE结构体中存在大量指针，但是里面的指针是用户空间的指针，而不是内核的
    char buf[120] = {0};
    int ret = fread(buf, sizeof(char), 100, fp);
    printf("fread = %s\n", buf);
    fclose(fp);

    // 内存分两类：内核区域 与 非内核区域/用户空间
    
    // 有缓冲文件流：
    // 用户空间有一块缓冲区 数据会首先存储在缓冲区 读数据的时候才会进入自己的数组
    
    // 无缓冲区文件流：                                                                            
    // 指的是，在用户空间，没有了缓冲区。 数据都是直接从内核中读取                                 
    // 这时候，只提供了一个按照数组读取。                                                          
    // 返回值问题：有缓冲文件流返回FILE*  无缓冲文件流返回了一个int即文件描述符fd                                                                 
    // 系统内核知道对应关系    
    return 0;
}

