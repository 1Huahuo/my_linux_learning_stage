#include <my_header.h>

int main(int argc, char* argv[]){                                  
    unsigned int x = 0x12345678;
    unsigned char *p = (unsigned char *)&x;

    printf("内存字节顺序：0x%X 0x%X 0x%X 0x%X\nx = 0x%X\n", p[0], p[1], p[2], p[3], x);

    if (p[0] == 0x78) {
        printf("当前主机是：小端序（主机字节序）\n");
    } else if (p[0] == 0x12) {
        printf("当前主机是：大端序\n");
    }

    return 0;
}
