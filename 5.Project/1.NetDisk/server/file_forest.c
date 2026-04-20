#include <my_header.h>
#include "file_forest.h"

// ---------------对外函数---------------
// 插入用户根目录
int forest_insert_root_dir(const char *user_name) {
    MYSQL *conn = sql_get_conn();
    if (!conn)
        return 0;

    char query[1024];
    snprintf(query, sizeof(query),
             "insert into file_forest (file_name, user_name, pdir_id, path, file_type, alive) "
             "values ('/', '%s', -1, '/', %d, 1)",
             user_name, FILE_TYPE_DIR);

    if (mysql_query(conn, query)) {
        /* 错误日志：fprintf(stderr, "forest_insert_root_dir: %s\n", mysql_error(conn)); */
        return 0;
    }

    return (int)mysql_insert_id(conn); // 获取当前数据库连接上，最近一次INSERT操作生成的AUTO_INCREMENT（自增主键）值
}

// 获取用户根目录id
int forest_get_root_dir_id(const char *user_name) {
    MYSQL *conn = sql_get_conn();
    if (!conn) return 0;

    char query[1024];
    snprintf(query, sizeof(query),
             "select id from file_forest where user_name='%s' and path='/' and file_type=%d and alive=1",
             user_name, FILE_TYPE_DIR);

    if (mysql_query(conn, query)) {
        /* 错误日志：fprintf(stderr, "forest_get_root_dir_id: %s\n", mysql_error(conn)); */
        return 0;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (!res) 
        return 0;

    int dir_id = 0;
    MYSQL_ROW row = mysql_fetch_row(res); // 返回字符串数组char**
    if (row) {
        dir_id = atoi(row[0]); // sql查询结果全为字符串
    }

    mysql_free_result(res);
    return dir_id;
}

// 简单路径解析：仅支持绝对路径（以 / 开头）和相对路径中的 ".." 与 "."
// 此函数为后续 cd 命令预留
int forest_resolve_path(const char *user_name, int base_dir_id, char *path) {
    int cur_id;
    char* p = path;

    if(p[0] == '/'){ // 绝对路径
        cur_id = forest_get_root_dir_id(user_name);
        if(cur_id == 0)
            return 0;
        p++; // 跳过'/'
    }else{ // 相对路径
        cur_id = base_dir_id;
    }

    // 剩余路径为空(cd  /)，直接返回当前目录id
    if (*p == '\0')
        return cur_id;

    // 使用可重入版本strtok_r按'/'分割
    char* saveptr;
    char* token = strtok_r(p,"/", &saveptr); // saveptr代表静态局部变量，所以需要传其地址
    while (token != NULL){
        if (strcmp(token, ".") == 0){ 
            // 处于当前目录，什么都不做
        } else if (strcmp(token, "..") == 0){ // 去上一级即当前父目录
            int parent = get_parent_id(cur_id);
            if(parent != -1){
                cur_id = parent;
            } // else(parent == -1) 即当前已在根目录，什么都不做
        } else { // 普通目录名
            int sub_id = find_subdir(user_name, cur_id, token);
            if(sub_id == 0){ // 目录不存在
                return 0;
            }
            cur_id = sub_id;
        }

        token = strtok_r(NULL, "/", &saveptr); // 不要忘记更新token！
    }

    return cur_id;
}

// 根据用户名与目录绝对路径返回目录id --- forest_resolve_path()的特殊情况(即固定以根目录开始) 
// 成功返回目录id，失败返回0
int forest_get_id_by_path(const char *user_name, const char *path) {
    // 注意：resolve 需要可修改的字符串，这里需要复制一份
    char tmp[1024];
    strncpy(tmp, path, sizeof(tmp)-1);
    tmp[sizeof(tmp)-1] = '\0';
    return forest_resolve_path(user_name, 0, tmp); // base_dir_id 在绝对路径下被忽略
}

// 根据id获取完整路径
bool forest_get_path_by_id(int dir_id, char *path_buf, size_t buf_size) {
    MYSQL *conn = sql_get_conn();
    if (!conn)
        return false;

    char query[1024];
    snprintf(query, sizeof(query),
             "select path from file_forest where id=%d and alive=1", dir_id);

    if (mysql_query(conn, query)) {
        /* 错误日志： fprintf(stderr, "forest_get_path_by_id: %s\n", mysql_error(conn)); */
        return false;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (!res) 
        return false;

    MYSQL_ROW row = mysql_fetch_row(res);
    if (row) {
        strncpy(path_buf, row[0], buf_size - 1);
        path_buf[buf_size - 1] = '\0';
        mysql_free_result(res);
        return true;
    }

    mysql_free_result(res);
    return false;
}

// 在指定父目录(pdir_id)下按名称查找子目录id
int find_subdir(const char *user_name, int pdir_id, const char *subdir_name){
    MYSQL *conn = sql_get_conn();
    if (!conn) 
        return 0;

    // 防注入
    char escaped_username[256];
    char escaped_subdirname[256];
    mysql_real_escape_string(conn, escaped_username, user_name, strlen(user_name));
    mysql_real_escape_string(conn, escaped_subdirname, subdir_name, strlen(subdir_name));
    char query[1024];
    snprintf(query, sizeof(query),
        "select id from file_forest "
        "where user_name='%s' AND pdir_id=%d AND file_name='%s' AND file_type=%d AND alive=1",
        escaped_username, pdir_id, escaped_subdirname, FILE_TYPE_DIR);

    /* printf("[DEBUG] SQL: %s\n", query); */
    if (mysql_query(conn, query)) { // 0成功，非0失败
        return 0;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (!res) 
        return 0;

    int id = 0;
    MYSQL_ROW row = mysql_fetch_row(res);
    /* printf("[Debug] row[0] = %s\n", row[0]); */
    if (row) {
        id = atoi(row[0]);
    }
    /* printf("[Dubug] return id = %d", id); */
    mysql_free_result(res);
    return id;
}

// 在指定父目录(pdir_id)下创建子目录(dir_name) --- 服务于 mkdir(主要)
int forest_mkdir(const char *user_name, int parent_id, const char *dir_name) { // 成功返回新目录id，失败返回0
    MYSQL *conn = sql_get_conn();
    if (!conn) 
        return 0;

    // 获取父目录完整路径
    char parent_path[512] = {0};
    if (!forest_get_path_by_id(parent_id, parent_path, sizeof(parent_path))) {
        return 0;
    }

    // 拼接新目录的完整路径
    char new_path[1024] = {};
    if (strcmp(parent_path, "/") == 0) { // 父目录为根目录
        snprintf(new_path, sizeof(new_path), "/%s", dir_name);
    } else { // 父目录为普通目录
        snprintf(new_path, sizeof(new_path), "%s/%s", parent_path, dir_name);
    }

    // 防注入
    char escaped_username[256] = {0};
    char escaped_dirname[256] = {0};
    char escaped_newpath[1024] = {0};
    mysql_real_escape_string(conn, escaped_username, user_name, strlen(user_name));
    mysql_real_escape_string(conn, escaped_dirname, dir_name, strlen(dir_name));
    mysql_real_escape_string(conn, escaped_newpath, new_path, strlen(new_path));

    // 将新目录插入文件森林表
    char query[2048] = {0};
    snprintf(query, sizeof(query),
        "insert into file_forest (file_name, user_name, pdir_id, path, file_type, alive) "
        "values ('%s', '%s', %d, '%s', %d, 1)",
        escaped_dirname, escaped_username, parent_id, escaped_newpath, FILE_TYPE_DIR);

    if (mysql_query(conn, query)) {
        /* 错误日志 */
        return 0;
    }

    return (int)mysql_insert_id(conn);
}

// 删除空目录
bool forest_rmdir(int dir_id) {
    MYSQL* conn = sql_get_conn();
    if(!conn)
        return false;
    // 检查目录下是否有子项目录/文件存在
    char check_query[1024] = {0};
    snprintf(check_query, sizeof(check_query), 
             "select count(*) from file_forest where pdir_id=%d AND alive=1", dir_id);
    if (mysql_query(conn, check_query)) {
        /* 错误日志：fprintf(stderr, "forest_rmdir check: %s\n", mysql_error(conn)); */
        return false;
    }
    
    MYSQL_RES* res = mysql_store_result(conn);
    if(!res)
        return false;

    MYSQL_ROW row = mysql_fetch_row(res);
    int count = row ? atoi(row[0]) : -1;
    mysql_free_result(res);

    if (count == -1)
        return false;
    if (count > 0) { // 目录不为空
        return false;
    } 
    // if count == 0

    // 软删除目录(alive = 0)
    char query[1024] = {0};
    snprintf(query, sizeof(query), 
             "update file_forest set alive=0 where id=%d AND file_type=%d AND alive=1", dir_id, FILE_TYPE_DIR);
    
    if (mysql_query(conn, query)) {
        /* 错误日志：fprintf(stderr, "forest_rmdir update: %s\n", mysql_error(conn)); */
        return false;
    }

    // 防止没有改动：即已经删除了alive=0，或者目标行不存在(update一个不存在的记录，MySQL不会报错)
    if (mysql_affected_rows(conn) == 0) { // 可能目录不存在或不是目录类型
        return false;
    }
    return true;
    // 即return mysql_affected_rows(conn) > 0;
}

// 获取目录(id)的父目录 id，若已是根目录则返回-1
int get_parent_id(int dir_id) { // 不考虑异常，默认返回-1
    MYSQL *conn = sql_get_conn();
    if (!conn) 
        return -1; // 查找失败，并非父目录不存在，但是不考虑此异常

    char query[1024];
    snprintf(query, sizeof(query),
        "SELECT pdir_id FROM file_forest WHERE id=%d AND alive=1", dir_id);

    if (mysql_query(conn, query)) { // query函数非0反而是失败，0代表无错误
        return -1;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (!res) 
        return -1;

    int parent = -1;
    MYSQL_ROW row = mysql_fetch_row(res);
    if (row) {
        parent = atoi(row[0]);
    }
    mysql_free_result(res);
    return parent;
}

// 列出指定目录(id)下的所有子项(目录/文件)，结果发送到client_fd
void forest_list_dir(int dir_id, const char *user_name, int client_fd){
    MYSQL *conn = sql_get_conn();
    if (!conn) {
        char msg[] = "ls: database error\n";
        send(client_fd, msg, strlen(msg), MSG_NOSIGNAL);
        return;
    }

    // 防注入
    char escaped_username[256];
    mysql_real_escape_string(conn, escaped_username, user_name, strlen(user_name));

    char query[1024];
    snprintf(query, sizeof(query),
        "select file_name, file_type from file_forest "
        "where user_name='%s' AND pdir_id=%d AND alive=1 "
        "ORDER BY file_type ASC, file_name ASC",
        escaped_username, dir_id);

    if (mysql_query(conn, query)) {
        char msg[] = "ls: query failed\n";
        send(client_fd, msg, strlen(msg), MSG_NOSIGNAL);
        return;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (!res) {
        char msg[] = "ls: no result\n";
        send(client_fd, msg, strlen(msg), MSG_NOSIGNAL);
        return;
    }

    // 打包所有信息
    char output[8192] = {0};
    int offset = 0;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        // 只读字符串常量，得加const修饰    row[0]-file_name    row[1]-file_type
        const char *type_mark = (atoi(row[1]) == FILE_TYPE_DIR) ? "[d]   " : "[f]   ";
        int len = snprintf(output + offset, sizeof(output) - offset,"%s%s\n", type_mark, row[0]);
        if (len < 0 || offset + len >= (int)sizeof(output) - 1) // 最多只能size-1个字符，末尾需要'\0' 
            break;
        offset += len;
    }
    mysql_free_result(res);

    if (offset == 0) {
        // 目录为空
        strcpy(output, "directory is empty\n");
    }

    send(client_fd, output, sizeof(output), MSG_NOSIGNAL);
}

// ------------------------------↓remove↓------------------------------
// 获取记录的类型和路径
bool forest_get_info_by_id(int id, int *file_type, char *path_buf, size_t buf_size) {
    MYSQL *conn = sql_get_conn();
    if (!conn) return false;

    char query[512];
    snprintf(query, sizeof(query),
        "SELECT file_type, path FROM file_forest WHERE id=%d AND alive=1", id);

    if (mysql_query(conn, query)) return false;
    MYSQL_RES *res = mysql_store_result(conn);
    if (!res) return false;
    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row) {
        mysql_free_result(res);
        return false;
    }
    if (file_type) *file_type = atoi(row[0]);
    if (path_buf) {
        strncpy(path_buf, row[1], buf_size - 1);
        path_buf[buf_size - 1] = '\0';
    }
    mysql_free_result(res);
    return true;
}

// 删除普通文件（软删除）
bool forest_remove_file(int file_id) {
    MYSQL *conn = sql_get_conn();
    if (!conn) return false;

    char query[512];
    snprintf(query, sizeof(query),
        "UPDATE file_forest SET alive=0 WHERE id=%d AND file_type=%d AND alive=1",
        file_id, FILE_TYPE_REG);

    if (mysql_query(conn, query)) return false;
    return mysql_affected_rows(conn) > 0;
}

// 递归删除目录：将该目录及其所有子孙节点标记为 alive=0
bool forest_remove_dir_recursive(int dir_id, const char *dir_path) {
    MYSQL *conn = sql_get_conn();
    if (!conn) return false;

    char escaped_path[1024];
    mysql_real_escape_string(conn, escaped_path, dir_path, strlen(dir_path));

    char pattern[1024];
    if (strcmp(dir_path, "/") == 0) {
        // 根目录：匹配所有以 '/' 开头的路径（实际上不应该删除根目录，但保留逻辑）
        snprintf(pattern, sizeof(pattern), "/%%");
    } else {
        snprintf(pattern, sizeof(pattern), "%s/%%", escaped_path);
    }

    // 开启事务（可选，保证原子性）
    mysql_query(conn, "START TRANSACTION");

    // 1. 删除目录自身
    char query1[512];
    snprintf(query1, sizeof(query1),
        "UPDATE file_forest SET alive=0 WHERE id=%d AND alive=1", dir_id);
    if (mysql_query(conn, query1)) {
        mysql_query(conn, "ROLLBACK");
        return false;
    }

    // 2. 删除所有子孙节点（利用 path LIKE pattern）
    char query2[1024];
    snprintf(query2, sizeof(query2),
        "UPDATE file_forest SET alive=0 WHERE path LIKE '%s' AND alive=1", pattern);
    if (mysql_query(conn, query2)) {
        mysql_query(conn, "ROLLBACK");
        return false;
    }

    mysql_query(conn, "COMMIT");
    return true;
}
// ------------------------------↑remove↑------------------------------
