## Day20——目录流

### 结构体

​		dirent 和 stat 结构体并非都只存目录项信息;**dirent 是目录项的 “浅层元信息”**，**stat 是文件 / 目录的 “完整属性信息”**。

1. **dirent**

   ```c
   // dirent是directory entry的简写，就是目录项的意思
   struct dirent {
    // 此目录项的inode编号，目录项中会存储文件的inode编号。一般是一个64位无符号整数（64位平台）
    ino_t          d_ino;
    // 到下一个目录项的偏移量。可以视为指向下一个目录项的指针(近似可以看成链表)，一般是一个64位有符号整数
    off_t          d_off;
    // 此目录项的实际大小长度，以字节为单位(注意不是目录项所表示文件的大小，也不是目录项结构体的大小)
    unsigned short d_reclen;       
    // 目录项所表示文件的类型，用不同的整数来表示不同的文件类型，可用于区分该目录项对应的文件是目录还是普通文件
    unsigned char  d_type;
    // 目录项所表示文件的名字，该字段一般决定了目录项的实际大小。也就是说文件名越长，目录项就越大
    char           d_name[256];
   };
   
   // d_type的可选值
   DT_BLK      // 块设备文件，对应整数值6
   DT_CHR      // 字符设备文件，对应整数值2
   DT_DIR      // 目录文件，对应整数值4
   DT_FIFO     // 有名管道文件，对应整数值1
   DT_LNK      // 符号链接文件，对应整数值10
   DT_REG      // 普通文件，对应整数值8
   DT_SOCK     // 套接字文件，对应整数值12
   DT_UNKNOWN  // 未知类型文件，对应整数值0
   ```

2. **stat**

   ```C
   struct stat {
       mode_t    st_mode;          // 包含文件的类型以及权限信息
       nlink_t   st_nlink;         // 文件的硬链接数量 
       uid_t     st_uid;           // 文件所有者的用户ID
       gid_t     st_gid;           // 文件所有者组的组ID
       off_t     st_size;          // 文件的实际大小, 以字节为单位
   
       struct timespec st_mtim;  /* 包含文件最后修改时间的结构体对象 */
   };
   
   // 最后修改时间st_mtim字段的类型稍微麻烦一些，是一个结构体timespec类型，代码如下：
   struct timespec {
       __time_t 			tv_sec;  // 时间戳，秒为单位。此类型别名一般就是long类型
       __syscall_slong_t   tv_nsec;            // 纳秒 - 存储时间戳当中不足秒的部分，用于精准表示时间。此类型别名一般就是long类型
   };
   ```

3. **passwd和group**

   ```c
   struct passwd {
       char *pw_name;   // 用户名
       char *pw_passwd; // 密码（通常是加密后的密码，但在现代系统中通常是 'x' 或 '*'，真正的密码存储在安全文件中）
       uid_t pw_uid;    // 用户ID
       gid_t pw_gid;    // 组ID
       char *pw_gecos;  // GECOS 字段，包含其他用户信息
       char *pw_dir;    // 用户主目录
       char *pw_shell;  // 用户登录 shell
   };
   
   struct group {
       char *gr_name;   // 组名
       char *gr_passwd; // 组密码（在现代系统中通常是 'x' 或 '*'，真正的密码存储在安全文件中）
       gid_t gr_gid;    // 组ID
       char **gr_mem;   // 包含组成员用户名的字符串数组
   };
   ```

4. **tm**

   ```c
   struct tm {
       int tm_sec;    /* 秒 – 取值区间为[0,59] */
       int tm_min;    /* 分 - 取值区间为[0,59] */
       int tm_hour;   /* 时 - 取值区间为[0,23] */
       int tm_mday;   /* 一个月中的日期 - 取值区间为[1,31] */
       int tm_mon;    /* 月份（从一月开始，0代表一月）- 取值区间为[0,11] */
       int tm_year;   /* 年份，其值从1900开始 */
       int tm_wday;   /* 星期 – 取值区间为[0,6]，星期日为0 */
       int tm_yday;   /* 从每年的1月1日开始的天数 – 取值区间为[0,365]，1月1日为0 */
       int tm_isdst;  /* 夏令时标识符，夏令时时为正，不是夏令时时为0，不了解情况时为负 */
   };
   ```

### **类型别名**

| 别名             | 原名           | 占位&说明 |
| :--------------- | :------------- | :-------- |
| d_ino            | unsigned long  | %lu       |
| off_t            | long           | %ld       |
|                  | unsigned short | %hu       |
|                  | unsigned char  | %hhu      |
| mode_t           | usigned int    | %u        |
| nlink_t          | usigned long   | %lu       |
| uid_t<br />gid_t | usigned int    | %u        |

### ==**相关函数**==

| 函数声明(返回值-函数名-参数)                                 | 头文件                                                       | 作用                                                         |
| :----------------------------------------------------------- | :----------------------------------------------------------- | :----------------------------------------------------------- |
| int chmod(const char *pathname, mode_t mode);                | #include <sys/stat.h>                                        | 改变文件的权限                                               |
| char *getcwd(char *buf, size_t size);                        | #include <unistd.h>                                          | 获取当前工作目录                                             |
| int chdir(const char *path);                                 | #include <unistd.h>                                          | 改变当前工作目录                                             |
| int mkdir(const char *pathname, mode_t mode);                | #include <sys/stat.h><br />#include <sys/types.h>            | 创建目录                                                     |
| int rmdir(const char *pathname);                             | #include <unistd.h>                                          | 删除目录                                                     |
| DIR ***opendir**(const char *name);<br />int closedir(DIR *dirp); | #include <dirent.h><br />#include <sys/types.h>              | 打开目录流<br />关闭目录流                                   |
| struct dirent *readdir(DIR *dirp);                           | #include <dirent.h>                                          | 通过目录流读取下一个目录项                                   |
| long telldir(DIR *dirp);<br />void seekdir(DIR *dirp, long loc); | \#include <dirent.h>                                         | 记录目录流指针的位置<br />返回记录的位置                     |
| void rewinddir(DIR *dirp);                                   | #include <dirent.h>                                          | 重置目录流的位置到开始处                                     |
| int stat(const char *path, struct stat *buf);                | #include <sys/stat.h>                                        | 用于获取指定文件的相关详细信息<br />主要包括各种元数据信息   |
| **POSIX标准库函数**<br />struct passwd *getpwuid(uid_t uid);<br />struct passwd *getpwuid(uid_t uid); | #include<sys/types.h><br/>#include <pwd.h><br />#include <grp.h> | 已经有用户UID以及组ID，则<br />将uid和gid成员转换成对应字符串 |
| struct tm *localtime(const time_t *timer);                   | #include <time.h>                                            | 处理最后修改时间戳                                           |
