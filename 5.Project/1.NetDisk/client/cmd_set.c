#include <my_header.h>
#include "cmd_set.h"

extern char cur_path[1024]; // 引用main.c中的全局变量

void cmd_cd(int fd){
    char response[1024] = {0};
    ssize_t ret = recv(fd, response, sizeof(response), 0);
    if (ret <= 0) {
        printf("cd: no response\n");
        return;
    }

    // 判断是否cd成功
    if (strcmp(response, "Directory changed.\n") == 0) {
        char new_path[1024] = {0};
        ret = recv(fd, new_path, sizeof(new_path) - 1, 0);
        if (ret > 0) {
            new_path[ret] = '\0';
            // ---------只显示最近的一层目录----------
            
            char* p = strrchr(new_path, '/');
            if (strcmp(p, "/") != 0) // 即new_path不为根目录(例如/test与/test1/test2)，才会处理
                p++; 
            strncpy(cur_path, p, sizeof(cur_path) - 1);
            // ---------------------------------------
            /* strncpy(cur_path, new_path, sizeof(cur_path) - 1); */
            cur_path[sizeof(cur_path) - 1] = '\0';
            printf("Directory changed.\n");
        } else {
            printf("Directory changed but cur_path lost.\n");
        }
    } else {
        // cd失败
        printf("%s", response);
    }
}

void cmd_ls(int fd){
    char response[8192] = {0};
    // 默认一次接收完(待改进)
    ssize_t ret = recv(fd, response, sizeof(response), 0);
    if (ret > 0) {
        response[ret] = '\0';
        printf("%s", response);
    } else {
        printf("ls: no response\n");
    }
}

void cmd_puts(int fd, char* local_path){

}

void cmd_gets(int fd, const char* remote_path){

}

void cmd_remove(int fd){
    char response[1024] = {0};
    ssize_t ret = recv(fd, response, sizeof(response), 0);
    if (ret > 0) {
        printf("%s", response);
    } else {
        printf("remove: no response\n");
    }
}

void cmd_pwd(int fd){
    char response[1024] = {0};
    ssize_t ret = recv(fd, response, sizeof(response), 0);
    if (ret > 0) {
        printf("%s\n", response); // 收到的消息无换行 
    } else {
        printf("pwd: no response\n");
    }
}

void cmd_mkdir(int fd){
    char response[1024] = {0};
    ssize_t ret = recv(fd, response, sizeof(response), 0);
    if (ret > 0) {
        printf("%s", response);
    } else {
        printf("mkdir: no response\n");
    }
}

void cmd_rmdir(int fd){
    char response[1024] = {0};
    ssize_t ret = recv(fd, response, sizeof(response), 0);
    if (ret > 0) {
        printf("%s", response);
    } else {
        printf("rmdir: no response\n");
    }
}
