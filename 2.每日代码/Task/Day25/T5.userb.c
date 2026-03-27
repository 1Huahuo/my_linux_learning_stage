#include <my_header.h>
#define BUF_SIZE 1024

// 辅助函数-判断当前目录是否存在
bool dir_exist(const char* path){
    struct stat st;
    return (stat(path, &st) == 0 && S_ISDIR(st.st_mode)); // stat成功返回0,失败返回-1
}

// 逐级创建目录（始终采用绝对路径，因为采用的是遍历完整路径的模式，所以使用绝对路径更方便）
void create_dirs(const char* file_path){
    char tmp[BUF_SIZE];
    snprintf(tmp, BUF_SIZE, "%s", file_path); // 安全的向字符串写函数
    
    int len = strlen(tmp);
    for(int i = 1; i < len; i++){ // i=1是为了跳过处理根目录'/'
        if(tmp[i] == '/'){
            tmp[i] = '\0'; // 通过置零截断字符串，可逐级得到绝对路径
            if(!dir_exist(tmp)){
                int ret = mkdir(tmp, 0755);
                ERROR_CHECK(ret, -1, "mkdir");
            }
            tmp[i] = '/'; // 当前级已检查，复原，继续制造下一级的绝对路径
        }   
    }
}

int main(int argc, char* argv[]){                                  
    int fd_pipe = open("1.pipe", O_RDONLY);
    ERROR_CHECK(fd_pipe, -1, "open pipe");
    
    // 读取路径长度
    int path_len;
    read(fd_pipe, &path_len, sizeof(int));
    // 读取路径
    char file_path[BUF_SIZE];
    read(fd_pipe, file_path, path_len);
    file_path[path_len] = '\0'; // 关键一步，尾部置零是为了在创建目录函数中更好的处理
    
    // 创建目录与文件
    create_dirs(file_path);
    int fd = open(file_path, O_WRONLY | O_CREAT | O_TRUNC, 0755);
    ERROR_CHECK(fd, -1, "open file");
    
    // 读取文件大小
    off_t file_size;
    read(fd_pipe, &file_size, sizeof(off_t));
    ftruncate(fd, file_size);
    // 读取文件内容
    char buf[BUF_SIZE];
    ssize_t count_read;
    while((count_read = read(fd_pipe, buf, BUF_SIZE)) > 0)
        write(fd, buf, count_read);

    close(fd);
    close(fd_pipe);
    printf("file:%s has received.\n", file_path);
    return 0;
}
