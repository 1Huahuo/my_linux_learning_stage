## Day36——网络编程3_epoll

1. **使用epoll的流程是怎么样，每一步操作会涉及哪些数据结构？**

   1. **创建epoll对象，得到epoll文件对象的文件描述符**

      ```c
      #include <sys/epoll.h>
      // open an epoll file descriptor
      int epoll_create(
          int size	// 历史遗留参数, 已无任何意义, 大于0即可
      );
      // 返回值: 成功返回epoll文件对象的文件描述符,  失败返回-1
      ```

   2. **调整监听事件的集合**

      ```c
      #include <sys/epoll.h>
      // control interface for an epoll file descriptor
      int epoll_ctl(
          int epfd,	//epoll的文件描述符
          int op,		//操作类型:EPOLL_CTL_ADD、EPOLL_CTL_MOD、EPOLL_CTL_DEL, 分别表示添加、修改和删除事件
          int fd,		//要被监听对应操作的文件描述符
          struct epoll_event *event	//指定对监听的文件描述符的监听事件。只有在添加、修改文件描述符时，这个参数才是必需的；在删除操作时，通常设置为NULL
      );
      // 返回值: 成功返回0,失败返回-1
      
      // (man 2 epoll_ctl)
      struct epoll_event
      {
          uint32_t     events;//事件: EPOLLIN(读取操作)、EPOLLOUT(写操作)、...、EPOLLET(边缘触发模式) (其他不重要)
         epoll_data_t data;	//上述事件的对应的文件描述符
      };
      
      typedef union epoll_data {
         void        *ptr;
         int          fd;	// 文件描述符
         uint32_t     u32;
         uint64_t     u64;
      } epoll_data_t;
      ```

   3. **进入阻塞状态，直到监听的设备就绪或者超时**

      ```c
      #include <sys/epoll.h>
      // wait for an I/O event on an epoll file descriptor
      int epoll_wait(
          int epfd,	                //epoll的文件描述符
          struct epoll_event *events,	//用于接收就绪集合的数组
          int maxevents,				//最大就绪集合长度
          int timeout	                //超时时间(毫秒), -1则一直等待
      );
      // 返回值: 成功返回就绪个数, 失败返回-1
      ```

2. **epoll的边缘触发与水平触发模式的区别是什么？然后使用代码区分这两种模式的区别？**

   1. **水平触发(EPOLLLT-默认)**

      - epoll的**默认**工作模式——==只要被监视的文件描述符上有待处理的事件，epoll_wait就会通知应用程序==

        > 以读事件为例子, 这意味着：如果某个文件描述符变得可读，epoll_wait会通知你这个文件描述符可读，即使你没有一次读完取缓冲区当中存在数据 ，下一次调用 epoll_wait时还会再次通知你就绪。

      - 代码验证：

        ```c
        #include <my_header.h>
        
        int main(int argc, char* argv[]){
            int epoll_fd = epoll_create(1);
        
            struct epoll_event event;
            event.events = EPOLLIN;
            event.data.fd = STDIN_FILENO;
            epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &event);
        
            while(1){
                struct epoll_event events;
                events.data.fd = -1;
                epoll_wait(epoll_fd, &events, 1, 0);
                int fd = events.data.fd;
        
                if(fd == STDIN_FILENO){
                    char buf[3] = {};
                    int ret_read = read(STDIN_FILENO, buf, sizeof(buf));
                    if(0 == ret_read){
                        printf("输入为空，终止进程");
                        break;
                    }else{
                        write(STDOUT_FILENO, buf, sizeof(buf));
                        printf("---此输出无换行---");
                        fflush_unlocked(stdout);
                    }
                }
            }
        
            close(epoll_fd);
            return 0;
        }
        ```

      - 代码实现效果

        ![水平触发](..\0.TyporaPicture\水平触发.png)

   2. **边缘触发(EPOLLET)**

      - 只在文件描述符==状态变化时==通知应用程序

        > 以读事件为例子，在边缘触发模式下：
        >
        > (1)如果缓冲区中存在数据，但是数据一直没有增多，那么epoll_wait在第一次触发之后就不会就绪，只有缓冲区的数据增多的时候，才能使epoll_wait就绪。
        >
        > (2)边缘触发的设置,  是在epoll_ctl的时候, 给struct epoll_event *event参数的events设置 | EPOLLET（即通过按位或来增加设置选项）

      - 代码验证：

        ```c
        #include <my_header.h>
        
        int main(int argc, char* argv[]){
            int epoll_fd = epoll_create(1);
        
            struct epoll_event event;
            event.events = EPOLLIN | EPOLLET;
            event.data.fd = STDIN_FILENO;
            epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &event);
        
            while(1){
                struct epoll_event events;
                events.data.fd = -1;
                epoll_wait(epoll_fd, &events, 1, 0);
                int fd = events.data.fd;
        
                if(fd == STDIN_FILENO){
                    char buf[3] = {};
                    int ret_read = read(STDIN_FILENO, buf, sizeof(buf));
                    if(0 == ret_read){
                        printf("输入为空，终止进程");
                        break;
                    }else{
                        write(STDOUT_FILENO, buf, sizeof(buf));
                        printf("---此输出无换行---");
                        fflush_unlocked(stdout);
                    }
                }
            }
        
            close(epoll_fd);
            return 0;
        }
        ```

      - 代码实现效果

        ![边缘触发](..\0.TyporaPicture\边缘触发.png)

   3. ==**recv与send都只能用于套接字，即无法读标准输入与写标准输出**==

