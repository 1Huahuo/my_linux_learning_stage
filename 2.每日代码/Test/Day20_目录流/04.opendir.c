#include <my_header.h>

int main(int argc, char *argv[]){
    DIR *dirp = opendir("/home/zhou/cpp71");
    ERROR_CHECK(dirp, NULL, "opendir failed");

    // 读目录流 读出的是目录中的子项即目录项。 
    struct dirent *dirent_p = readdir(dirp);
    // dirent里面的字段 
    printf("%ld %s %ld\t  %d %d\n", 
           dirent_p->d_ino, dirent_p->d_name, dirent_p->d_off, 
           dirent_p->d_reclen, dirent_p->d_type);

    closedir(dirp); // 不要忘记关闭流
    return 0;
}

