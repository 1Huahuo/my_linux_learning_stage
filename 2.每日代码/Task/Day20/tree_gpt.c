#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>

void print_tree(const char *path, const char *prefix, bool is_last) {
    DIR *dir = opendir(path);
    if (!dir) return;

    struct dirent *entry;
    int count = 0;
    int total = 0;

    // 先统计总文件数，用于判断最后一个文件
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
            total++;
    }
    rewinddir(dir);

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        count++;
        bool last = (count == total);

        // 打印前缀和分支符号
        printf("%s", prefix);
        printf("%s── %s\n", last ? "└" : "├", entry->d_name);

        // 构造完整路径
        char fullpath[1024];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

        // 如果是目录，递归调用
        struct stat st;
        if (stat(fullpath, &st) == 0 && S_ISDIR(st.st_mode)) {
            // 更新前缀：最后一个目录用空格，否则用竖线
            char new_prefix[1024];
            snprintf(new_prefix, sizeof(new_prefix), "%s%s  ", prefix, last ? "   " : "│");
            print_tree(fullpath, new_prefix, last);
        }
    }
    closedir(dir);
}

int main(int argc, char *argv[]) {
    const char *start_path = argc > 1 ? argv[1] : ".";
    printf("%s\n", start_path);
    print_tree(start_path, "", true);
    return 0;
}
