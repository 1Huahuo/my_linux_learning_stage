#include <my_header.h>

// 聊天室服务端
typedef struct client_s
{
    int fd;
    int is_connect;
    time_t last_active_time;
} client_t;

int main(int argc,char* argv[])
{
    //启动socket函数, 获得对应的socket_fd描述符
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // 端口复用使得服务器可以直接重启，端口不会被系统接管 
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    //创建服务端的地址和端口
    struct sockaddr_in sockaddr;
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = inet_addr("192.168.85.128");
    sockaddr.sin_port = htons(atoi("12345"));

    //绑定端口ip
    bind(server_fd, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
    //监听端口
    listen(server_fd, 10);

    int epoll_fd = epoll_create(1);
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = server_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event);

    //客户端数组: 用于保存所有链接服务器的客户端
    client_t client_list[1024];
    memset(client_list, 0, sizeof(client_list));
    int idx = 0; // 曾经连接的数量
    while(1)
    {
        struct epoll_event events[100] = {};
        int ready_count = epoll_wait(epoll_fd, events, 100, 0);

        for(int i = 0; i < ready_count; i++){
            int fd = events[i].data.fd;
            // 判断是否有socket数据达到:即socket就绪
            if(fd == server_fd){
                //获得一个新的客户端链接
                int client_fd = accept(server_fd, NULL, NULL);
                //保存到"客户端"连接数组中
                // client_list[idx].last_active_time = time(NULL);
                time(&client_list[idx].last_active_time);
                client_list[idx].is_connect = 1;
                client_list[idx].fd = client_fd;
                // 加入之后的监听集合
                event.data.fd = client_fd;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event);
                idx++;
            }else{ // 群聊服务器不监听标准输入
                char buf[60] = {0};
                int ret_recv = recv(fd, buf, sizeof(buf), 0);
                // 如果返回值为0, 说明这个客户端已经断开，（客户端断开也是一种读事件)
                if(ret_recv == 0)
                {
                    //把客户端数组中该标记置为0: 即非存活状态
                    for(int j = 0; j < idx; j++)
                        if(fd == client_list[j].fd)
                            client_list[j].is_connect = 0;
                    //取消后续对此客户端的select监听(防止死循环)
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                    // 关闭这个客户端
                    close(fd);
                    printf("一个客户端断开连接\n");
                }
                else{ // 代表当前客户端发送的是有效信息，则把读取到的数据依次发给其余的客户端
                      //遍历所有客户端
                    for(int j = 0; j < idx; j++)
                    {
                        if(client_list[j].is_connect == 0 || client_list[j].fd == fd)
                        {
                            //跳过断开连接的客户端 与 发送信息的客户端
                            continue;
                        }
                        //发送信息
                        send(client_list[j].fd, buf, sizeof(buf), 0);
                    }
                    // 更新此客户端的最后活跃时间
                    // client_list[i].last_active_time = time(NULL);
                    for(int j = 0; j < idx; j++)
                        if(fd == client_list[j].fd)
                            time(&client_list[j].last_active_time);
                }
            }
        }
        // 判断哪个客户端的最后活跃时间距离现在已超过10秒
        for(int i = 0; i < idx; i++){
            time_t now;
            time(&now);
            if(client_list[i].is_connect == 1 && now - client_list[i].last_active_time > 10){
                // 踢出连接，客户端会收到一个连接断开信息
                close(client_list[i].fd);
                client_list[i].is_connect = 0;
                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_list[i].fd, NULL);
                printf("一个客户端已经不活跃超过10秒，断开与其的连接\n");
            }
        }
    }

    close(server_fd);

    return 0;
}
