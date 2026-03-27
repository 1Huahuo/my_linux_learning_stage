#include <my_header.h>

/*

*/
int main(int argc, char* argv[]){                                  
    ARGS_CHECK(argc, 2);
    int fd = open(argv[1], O_RDWR);
    ERROR_CHECK(fd, -1, "open");
    
    // 截断占位
    int ret = ftruncate(fd, 5);
    ERROR_CHECK(ret, -1, "ftrucate");

    char* p = (char*)mmap(NULL, 5, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    ERROR_CHECK(p, NULL, "mmap");
    
    for(int i = 0; i < 5; i++)
        printf("%c", p[i]);
    printf("\n");
    
    p[4] = '0';
    printf("----------------------------\n");
    for(int i = 0; i < 5; i++)
        printf("%c", p[i]);
    printf("\n");

    // 先接触内存映射，再关闭文件描述符
    munmap(p, 5);
    close(fd);    

    return 0;
}
