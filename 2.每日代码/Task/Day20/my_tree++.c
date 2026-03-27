#include <my_header.h>

static int dirs = 0;
static int files = 0;

void dfs_print(char* dirpath, char* prefix){
    DIR* dirp = opendir(dirpath);
    ERROR_CHECK(dirp, NULL, "opendir");

    struct dirent* dirent_p;
    struct dirent* entries[1024];
    int entry_count = 0;
    
    // 先收集所有条目
    while((dirent_p = readdir(dirp)) != NULL){
        if(strcmp(dirent_p->d_name, ".") == 0 || strcmp(dirent_p->d_name, "..") == 0)
            continue;
        entries[entry_count++] = dirent_p;
    }
    
    // 遍历所有条目
    for(int i = 0; i < entry_count; i++){
        dirent_p = entries[i];
        
        // 判断是否为最后一个条目
        int is_last = (i == entry_count - 1);
        
        // 打印前缀和连接线
        printf("%s", prefix);
        if(is_last){
            printf("└── ");
        } else {
            printf("├── ");
        }
        printf("%s\n", dirent_p->d_name);
        
        // 构造完整路径
        char path[1024] = {0};
        sprintf(path, "%s/%s", dirpath, dirent_p->d_name);
        
        // 如果是目录，递归遍历
        if(dirent_p->d_type == DT_DIR){
            dirs++;
            char new_prefix[1024] = {0};
            if(is_last){
                sprintf(new_prefix, "%s    ", prefix);  // 最后一个，用空格
            } else {
                sprintf(new_prefix, "%s│   ", prefix);  // 不是最后一个，用竖线
            }
            dfs_print(path, new_prefix);
        } else {
            files++;
        }
    }
    closedir(dirp);
}

int main(int argc, char *argv[]){
    ARGS_CHECK(argc, 2);
    printf("%s\n", argv[1]);
    dfs_print(argv[1], "");  // 初始前缀为空
    
    printf("\n%d directories, %d files\n", dirs, files);
    
    return 0;
}
