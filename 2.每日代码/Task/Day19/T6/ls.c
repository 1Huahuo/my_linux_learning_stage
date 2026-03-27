#include <my_header.h>

// dirent是directory entry的简写，就是目录项的意思

int main(int argc, char *argv[]){
    ARGS_CHECK(argc, 2);
    DIR* dirp = opendir(argv[1]);
    ERROR_CHECK(dirp, NULL, "opendir");
    struct dirent* p_dirent;
    // 循环度读目录项并打印目录信息
    while((p_dirent = readdir(dirp)) != NULL){ // dirp 类似与 fp 是目录文件指针
        printf("inode num = %lu, reclen = %hu, type = %u, name = %s\n", // %lu--long无符整，%hu--short无符整
                p_dirent->d_ino, 
                p_dirent->d_reclen, 
                p_dirent->d_type, 
                p_dirent->d_name);
    }
    closedir(dirp);
    return 0;
}
