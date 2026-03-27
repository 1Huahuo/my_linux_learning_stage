#include <dirent.h>
#include <my_header.h>
#include <stdio.h>

int main(int argc, char *argv[]){
    DIR *dirp = opendir(argv[1]);
    ERROR_CHECK(dirp, NULL, "opendir failed");

    struct dirent *dirent_p = NULL;
    while((dirent_p = readdir(dirp)) != NULL){
        printf("%ld %d %s\n", dirent_p->d_ino, dirent_p->d_type, 
               dirent_p->d_name);

    }
    closedir(dirp);

    return 0;
}

