#include <my_header.h>

/* Usage:  */
int main(int argc, char *argv[]){
    ARGS_CHECK(argc, 2);
    DIR* dirp = opendir(argv[1]);
    ERROR_CHECK(dirp, NULL, "opendir");   

    struct dirent* dirent_p = NULL;
    while((dirent_p = readdir(dirp)) != NULL){
        printf("inode = %lu, reclen = %hu, type = %u, name = %s\n",
                dirent_p->d_ino,
                dirent_p->d_reclen, 
                dirent_p->d_type, 
                dirent_p->d_name);
    }
    closedir(dirp);
    return 0;
}
