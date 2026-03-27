#include <my_header.h>

int main(int argc, char *argv[]){
    // stat 是用于获取详细信息的函数。 
    /* struct stat *stat_p; */
    /* struct stat *stat_p = NULL; */
    // 上面的写法是错的,野指针和空指针本质没有有效的对应的空间，正确的两种写法: 
    /* struct stat *stat_p = malloc(sizeof(struct stat)); */
    struct stat stat_buf;
    struct stat *stat_p = &stat_buf;

    int ret = stat("1.txt", stat_p);
    ERROR_CHECK(ret, -1, "stat failed");
    printf("%o %ld %d %d %ld %ld\n",
           stat_p->st_mode, stat_p->st_nlink,
           stat_p->st_uid, stat_p->st_gid,
           stat_p->st_size, stat_p->st_mtim.tv_sec);

    /* stat_p->st_gid; */
    /* stat_p->st_mode; // st_mode : 文件类型 + 权限 */ 
    /* stat_p->st_nlink; // 硬链接数 */
    /* stat_p->st_uid; */
    /* stat_p->st_gid; */
    /* stat_p->st_size; */
    /* stat_p->st_mtim.tv_sec; */

    return 0;
}
