#include <my_header.h>

void dirent_name(char* dirent_name){
    int i = -1;
    while(dirent_name[++i] != '\0'){
        if(dirent_name[i] >= 'A' && dirent_name[i] <= 'Z')
            dirent_name[i] = dirent_name[i] + 'a' - 'A';
    }
}

int cmp(const void * a, const void* b){
    struct dirent* dirent_p1 = *(struct dirent**)a;
    struct dirent* dirent_p2 = *(struct dirent**)b;

    char dirent_name1[256];
    char dirent_name2[256];
    
    strncpy(dirent_name1, dirent_p1->d_name, sizeof(dirent_name1) - 1);
    strncpy(dirent_name2, dirent_p2->d_name, sizeof(dirent_name2) - 1);
    dirent_name1[sizeof(dirent_name1) - 1] = '\0';
    dirent_name2[sizeof(dirent_name2) - 1] = '\0';

    dirent_name(dirent_name1);
    dirent_name(dirent_name2);

    return strcmp(dirent_name1, dirent_name2);
}

void set_type_mode(mode_t mode, char *tm_str){
    switch (mode & S_IFMT) {
        case S_IFBLK:   tm_str[0] = 'b';        break;
        case S_IFCHR:   tm_str[0] = 'c';        break;
        case S_IFDIR:   tm_str[0] = 'd';        break;
        case S_IFIFO:   tm_str[0] = 'p';        break;
        case S_IFLNK:   tm_str[0] = 'l';        break;
        case S_IFREG:   tm_str[0] = '-';        break;
        case S_IFSOCK:  tm_str[0] = 's';        break;
        default:        tm_str[0] = '?';        break;
    }
    tm_str[1] = (mode & 0400) ? 'r' : '-';
    tm_str[2] = (mode & 0200) ? 'w' : '-';
    tm_str[3] = (mode & 0100) ? 'x' : '-';
    tm_str[4] = (mode & 0040) ? 'r' : '-';
    tm_str[5] = (mode & 0020) ? 'w' : '-';
    tm_str[6] = (mode & 0010) ? 'x' : '-';
    tm_str[7] = (mode & 0004) ? 'r' : '-';
    tm_str[8] = (mode & 0002) ? 'w' : '-';
    tm_str[9] = (mode & 0001) ? 'x' : '-';
    tm_str[10] = '\0'; 
}

void set_time(__time_t mtime, char *time_str){
    const char month_arr[][10] = {
        "1月", "2月", "3月", "4月", "5月", "6月",
        "7月", "8月", "9月", "10月", "11月", "12月"
    };
    struct tm* st_tm = localtime(&mtime);
    sprintf(time_str, "%s %2d %02d:%02d",
            month_arr[st_tm->tm_mon],
            st_tm->tm_mday,
            st_tm->tm_hour,
            st_tm->tm_min);
}

int main(int argc, char *argv[]){
    char* path = NULL;
    if(argc == 1)
        path = ".";
    else{
        ARGS_CHECK(argc, 2);
        path = argv[1];
    }

    DIR* dirp = opendir(path);
    ERROR_CHECK(dirp, NULL, "opendir");

    struct dirent* dirent_p = NULL;
    int count = 0;
    while((dirent_p = readdir(dirp)) != NULL)
        count++;

    struct dirent** dirent_p_arr = (struct dirent**)malloc(count * sizeof(struct dirent*));
    ERROR_CHECK(dirent_p_arr, NULL, "malloc dirent point arr");
    
    rewinddir(dirp);
    for(int i = 0; (dirent_p = readdir(dirp)) != NULL; i++)
        dirent_p_arr[i] = dirent_p;
    
    qsort(dirent_p_arr, count, sizeof(struct dirent*), cmp);

    for(int i = 0; i < count; i++){
        struct stat sb;
        int ret = stat(dirent_p_arr[i]->d_name, &sb);
        ERROR_CHECK(ret, -1, "stat");
        // 1.处理stat的成员st_mode，将它转换成权限和类型字符串
        char tm_str[1024] = {0};
        set_type_mode(sb.st_mode, tm_str);

        // 2.获取用户名和组名
        char *username = getpwuid(sb.st_uid)->pw_name;
        char *gname = getgrgid(sb.st_gid)->gr_name;

        // 3.将时间戳转换为时间字符串
        char time_str[1024] = {0};
        set_time(sb.st_mtim.tv_sec, time_str);

        printf("%s %2lu %s %s %7lu %s %s\n",
               tm_str,
               sb.st_nlink,
               username,
               gname,
               sb.st_size,
               time_str,
               // 文件名不再从dirent结构体中获取，不要忘记修改
               dirent_p_arr[i]->d_name);
    }

    free(dirent_p_arr);
    closedir(dirp);
    return 0;
}
