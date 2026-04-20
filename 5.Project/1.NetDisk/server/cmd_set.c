#include <my_header.h>
#include "cmd_set.h"

void cmd_cd(Session* sess, char* path){
    /* printf("[DEBUG] cd: user='%s', cur_dir=%d, path='%s'\n", sess->user_name, sess->cur_dir_id, path); */
    int target_id = forest_resolve_path(sess->user_name, sess->cur_dir_id, path);
    if(0 == target_id){
        char msg[] = "cd: no such directory\n";
        send(sess->fd, msg, strlen(msg), MSG_NOSIGNAL);
        return;
    }
    sess->cur_dir_id = target_id;
    
    // 发送成功消息
    char msg[] = "Directory changed.\n";
    send(sess->fd, msg, strlen(msg), MSG_NOSIGNAL);

    // 紧接着发送新路径
    char new_path[1024] = {0};
    if (forest_get_path_by_id(target_id, new_path, sizeof(new_path))) {
        send(sess->fd, new_path, strlen(new_path), MSG_NOSIGNAL);
    } else {
        // 获取失败时发送空字符
        char empty = '\0';
        send(sess->fd, &empty, 1, MSG_NOSIGNAL);
    }
}

void cmd_ls(Session* sess, char* path){
    // 简单实现忽略path，只列出当前目录
    // 支持ls path需要调用forest_resolve_path获取目标目录id
    forest_list_dir(sess->cur_dir_id, sess->user_name, sess->fd);
}

void cmd_puts(Session* sess, char* path){
    
}

void cmd_gets(Session* sess, char* path){
    
}

void cmd_remove(Session* sess, char* path){
    // 1. 解析路径得到目标 id
    int target_id = forest_resolve_path(sess->user_name, sess->cur_dir_id, path);
    if (target_id == 0) {
        char msg[] = "remove: no such file or directory\n";
        send(sess->fd, msg, strlen(msg), MSG_NOSIGNAL);
        return;
    }

    // 2. 禁止删除根目录
    if (get_parent_id(target_id) == -1) {
        char msg[] = "remove: cannot remove root directory\n";
        send(sess->fd, msg, strlen(msg), MSG_NOSIGNAL);
        return;
    }

    // 3. 获取目标的类型和路径
    int file_type;
    char target_path[1024] = {0};
    if (!forest_get_info_by_id(target_id, &file_type, target_path, sizeof(target_path))) {
        char msg[] = "remove: failed to get file info\n";
        send(sess->fd, msg, strlen(msg), MSG_NOSIGNAL);
        return;
    }

    bool success = false;
    if (file_type == FILE_TYPE_REG) {
        success = forest_remove_file(target_id);
    } else if (file_type == FILE_TYPE_DIR) {
        success = forest_remove_dir_recursive(target_id, target_path);
    } else {
        char msg[] = "remove: unknown file type\n";
        send(sess->fd, msg, strlen(msg), MSG_NOSIGNAL);
        return;
    }

    if (success) {
        char msg[] = "remove success\n";
        send(sess->fd, msg, strlen(msg), MSG_NOSIGNAL);
        log_operation("User %s removed %s", sess->user_name, target_path);
    } else {
        char msg[] = "remove: failed\n";
        send(sess->fd, msg, strlen(msg), MSG_NOSIGNAL);
    }
}

void cmd_pwd(Session* sess){
    char path[1024] = {0};
    if (forest_get_path_by_id(sess->cur_dir_id, path, sizeof(path))) {
        send(sess->fd, path, sizeof(path), MSG_NOSIGNAL); // 完整路径'\0'结尾，无换行符
        return;
    } else {
        char msg[] = "pwd: get current directory failed\n";
        send(sess->fd, msg, strlen(msg), MSG_NOSIGNAL);
        return;
    }   
}

// ！暂不支持递归创建
void cmd_mkdir(Session* sess, char* path){
    // 拆分目标路径的父目录和目录名
    char *last_slash = strrchr(path, '/'); // 倒着找第一个 '/'
    char dir_name[256] = {0};
    int parent_id;

    if (last_slash == NULL) { // 找不到 '/' 返回 NULL
        // 相对路径,则父目录为当前工作目录
        strncpy(dir_name, path, sizeof(dir_name) - 1);
        parent_id = sess->cur_dir_id;
    } // 接下来即为绝对路径 
    else if (last_slash == path){ // 路径在根目录下 —— 即 /dir_name
        strncpy(dir_name, last_slash + 1, sizeof(dir_name) - 1);
        parent_id = forest_get_root_dir_id(sess->user_name);
    } 
    else { // 父目录为普通多级目录
        *last_slash = '\0'; // 分离父目录与待插入目录名  
        parent_id = forest_resolve_path(sess->user_name, sess->cur_dir_id, path);
        *last_slash = '/';   // 恢复现场(回溯)
        strncpy(dir_name, last_slash + 1, sizeof(dir_name) - 1);
    }

    if (parent_id == 0) {
        char msg[] = "mkdir: parent directory dont exist\n";
        send(sess->fd, msg, strlen(msg), MSG_NOSIGNAL);
        return;
    }

    // 检查目录是否存在
    if (find_subdir(sess->user_name, parent_id, dir_name) != 0) {
        char msg[] = "mkdir: directory already exist\n";
        send(sess->fd, msg, strlen(msg), MSG_NOSIGNAL);
        return;
    }

    // 创建目录并插入文件森林表
    int new_id = forest_mkdir(sess->user_name, parent_id, dir_name);
    if (new_id == 0) {
        char msg[] = "mkdir: create directory failed\n";
        send(sess->fd, msg, strlen(msg), MSG_NOSIGNAL);
        return;
    }
    char msg[] = "mkdir success\n";
    send(sess->fd, msg, strlen(msg), MSG_NOSIGNAL);
    log_operation("User %s created directory: %s", sess->user_name, dir_name);
    return;
}

void cmd_rmdir(Session* sess, char* path){
    // 解析目标目录获取其id
    // forest_resolve_path支持相对路径(还支持.与..)和绝对路径
    int target_id = forest_resolve_path(sess->user_name, sess->cur_dir_id, path); 
    printf("[DEBUG] rmdir target_id = %d\n", target_id);
    if (target_id == 0) {
        char msg[] = "rmdir: no such directory\n";
        send(sess->fd, msg, strlen(msg), MSG_NOSIGNAL);
        return;
    }

    // 不能删除根目录(即pdir_id=-1)
    if (get_parent_id(target_id) == -1) {
        char msg[] = "rmdir: cant remove root directory\n";
        send(sess->fd, msg, strlen(msg), MSG_NOSIGNAL);
        return;
    }

    // 软删除
    if (forest_rmdir(target_id)) {
        char msg[] = "rmdir success\n";
        send(sess->fd, msg, strlen(msg), MSG_NOSIGNAL);
        log_operation("User %s removed directory id=%d", sess->user_name, target_id);
    } else {
        char msg[] = "rmdir: failed---directory not empty or not a directory)\n";
        send(sess->fd, msg, strlen(msg), MSG_NOSIGNAL);
    }
}

