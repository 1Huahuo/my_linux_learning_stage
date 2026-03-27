#include <my_header.h>

/*

*/
int main(int argc, char* argv[]){                                  
    int fd = open("3.pipe", O_RDONLY);

    char name[100] = {0};
    char str[1024] = {0};
    int length;

    read(fd, name, sizeof(name) - 1);
    read(fd, &length, sizeof(int));
    read(fd, str, sizeof(str) - 1);

    mkdir("new_dir", 0777); // 注意若是没有执行权限，则无法切换到此目录
    chdir("new_dir");
    int new_fd = open(name, O_WRONLY | O_CREAT, 0666); // 加了创建标志一定要添加权限，不然会给.txt文件创建成可执行文件了
    ftruncate(new_fd, length);

    write(new_fd, str, length);
    return 0;
}

