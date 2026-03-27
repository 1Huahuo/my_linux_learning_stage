#include <my_header.h>

void set_type_mode(mode_t mode, char* tm_str){
    switch (mode & S_IFMT){ // 与八进制数进行与运算
    case S_IFBLK:   tm_str[0] = 'b';    break;  
    case S_IFCHR:   tm_str[0] = 'c';    break;  
    case S_IFDIR:   tm_str[0] = 'd';    break;  
    case S_IFIFO:   tm_str[0] = 'p';    break;  
    case S_IFLNK:   tm_str[0] = 'l';    break;  
    case S_IFREG:   tm_str[0] = '-';    break;  
    case S_IFSOCK:  tm_str[0] = 's';    break;  
    default:        tm_str[0] = '?';    break;
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

void set_time(__time_t mtim_sec, char* time_str){
    const char month_arr[][10] = {"1月", "2月", "3月", "4月", "5月", "6月", "7月", "8月", "9月",
        "10月", "11月", "12月"}; // uft-8中文字符占3个字节 
    struct tm* tm = localtime(&mtim_sec);
    sprintf(time_str, "%s %2d %02d:%02d", 
            month_arr[tm->tm_mon], 
            tm->tm_mday, 
            tm->tm_hour, 
            tm->tm_min);
}

/* Usage:  */
int main(int argc, char *argv[]){                                  
    char* dir_name = NULL; // 存储目录名
    if(argc == 1){
        dir_name = ".";
    }else{
        ARGS_CHECK(argc, 2);
        dir_name = argv[1];
    }

    DIR* dirp = opendir(dir_name);
    ERROR_CHECK(dirp, NULL, "opendir");

    struct dirent* dirent_p = NULL;
    while((dirent_p = readdir(dirp)) != NULL){
        struct stat stat_buff;
        int ret = stat(dirent_p->d_name, &stat_buff);
        ERROR_CHECK(ret, -1, "stat");
        
        char mode_str[11] = {0};
        set_type_mode(stat_buff.st_mtim.tv_sec, mode_str);

        char time_str[1024] = {0};
        set_time(stat_buff.st_mtim.tv_sec, time_str);

        printf("%s %2lu %s %s %6lu %s %s\n", 
                mode_str, 
                stat_buff.st_nlink,
                getpwuid(stat_buff.st_uid)->pw_name, 
                getgrgid(stat_buff.st_gid)->gr_name, 
                stat_buff.st_size, 
                time_str, 
                dirent_p->d_name);
    }
    
    closedir(dirp);
    return 0;
}

