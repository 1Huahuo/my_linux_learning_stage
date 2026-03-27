#include <my_header.h>

static int dirs = 0;
static int files = 0;

void dfs_print(char* dirpath, int width){
    DIR* dirp = opendir(dirpath);
    ERROR_CHECK(dirp, NULL, "opendir");

    struct dirent* dirent_p;
    while((dirent_p = readdir(dirp)) != NULL){
        if(strcmp(dirent_p->d_name, ".") == 0 || strcmp(dirent_p->d_name, "..") == 0)
            continue;

        printf("└");
        for(int i = 1; i < width; i++)
            printf("─");
        printf("%s\n", dirent_p->d_name);
        if(dirent_p->d_type == DT_DIR){
            dirs++;
            char path[1024] = {0};
            sprintf(path, "%s/%s", dirpath, dirent_p->d_name);
            dfs_print(path, width + 4);
        }else{
            files++;
        }
    }
    closedir(dirp);
}

/* Usage:  */
int main(int argc, char *argv[]){                                  
    ARGS_CHECK(argc, 2);
    printf("%s\n", argv[1]);
    dfs_print(argv[1], 4); // 4为递归一层加的宽度
    printf("\n%d direcatories, %d files\n", dirs, files);
    
    return 0;
}

