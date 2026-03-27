#include <my_header.h>

int main(int argc, char *argv[]){
    int fd = open("777_copy.txt", O_RDWR);

    ftruncate(fd, 100);
    
    // 在下载时用于占位。 
    // 防止下载过程中，下载过程中空间被占 

    return 0;
}

