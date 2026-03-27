#include <my_header.h>

void print_dirent(const struct dirent* d){
    if(d == NULL)
        return;
    printf("目录项信息：\n");
    // 1. inode号（文件唯一标识）
    printf("d_ino:%lu(inode编号)\n", d->d_ino);
    // 2. 下一个目录项偏移量
    printf("d_off:%ld\n", d->d_off);
    // 3. 目录项记录长度（字节）
    printf("d_reclen:%hu(目录项记录长度)\n", d->d_reclen);
    // 4. 类型标识（部分系统有，POSIX标准字段）
    printf("d_type:%u\n", d->d_type);
    // 5. 文件名（核心字段）
    printf("d_name:%s(文件名/目录名)\n", d->d_name);
    
}

int main(int argc, char *argv[]){
    ARGS_CHECK(argc, 2);
    DIR* dirp = opendir(argv[1]);
    ERROR_CHECK(dirp, NULL, "opendir");
    struct dirent* p_dirent;
    // 循环读目录项并打印目录信息
    while((p_dirent = readdir(dirp)) != NULL){ // dirp 类似与 fp 是目录文件指针
       print_dirent(p_dirent); 
    }
    
    closedir(dirp);
    return 0;
}
