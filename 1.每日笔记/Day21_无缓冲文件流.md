## Day21——无缓冲文件流

### 类型别名

|  别名   |        原名(32/64)         | 格式符 |
| :-----: | :------------------------: | :----: |
| size_t  | unsigned int/unsigned long |  %zu   |
| ssize_t |       long/long long       |  %zd   |

### 相关函数

| int open(const char *pathname, int flags);<br />int open(const char *pathname, int flags, mode_t mode); | #include <fcntl.h> <br/>#include <sys/types.h>  #include <sys/stat.h> | 打开一个已存在的文件或创建一个新文件                     |
| :----------------------------------------------------------- | :----------------------------------------------------------- | :------------------------------------------------------- |
| int close(int fd);                                           | #include <unistd.h>                                          | 关闭文件描述符，而不是立刻释放文件对象的资源             |
| ssize_t read(int fd, void *buf, size_t count);               | #include <unistd.h>                                          | 用户进程从内核区域中直接将数据读取出来                   |
| ssize_t write(int fd, const void *buf, size_t count);        | #include <unistd.h>                                          | 用户进程直接将内存数据写入到内核区域中                   |
| int ftruncate(int fd, off_t length);                         | #include <unistd.h>                                          | 改变已打开文件的长度，这个函数允许你增大或减小文件的大小 |
| off_t lseek(int fd, off_t offset, int whence);               | \#include <unistd.h>                                         | 修改当前文件描述符（fd）对应的读写位置                   |
