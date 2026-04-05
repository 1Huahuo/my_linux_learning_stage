#include <my_header.h>

// 聊天室客户端
int main(int argc,char*argv[])
{
    // 启动socket函数, 获得对应的socket_fd描述符
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);

    // 创建服务端的地址和端口
    struct sockaddr_in sockaddr;
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = inet_addr("192.168.85.128");
    sockaddr.sin_port = htons(atoi("12345"));

    // 建立socket链接
    connect(client_fd, (struct sockaddr *)&sockaddr, sizeof(sockaddr));

    // 构建select监听的集合: fd_set类型
    fd_set set;
    // 清空set: 初始化set
    FD_ZERO(&set);

    while(1)
    {
        // 添加socket监听
        FD_SET(client_fd, &set);
        // 添加标准输入监听集合
        FD_SET(STDIN_FILENO, &set);
        // select开始监听有没有就绪事件
        select(10, &set, NULL, NULL, NULL);

        // 判断标准输入是否就绪
        if(FD_ISSET(STDIN_FILENO, &set))
        {
            // 读取标准输入
            char buf[60] = {0};
            int res_read = read(STDIN_FILENO, buf, sizeof(buf));
            // 用户输入EOF(Ctrl+D), 准备挂断链接
            if(res_read == 0)
            {
                printf("主动退出群聊 \n");
                break;
            }
            // 把读取的标准输入, 发送给服务器
            send(client_fd, buf, sizeof(buf), 0);
        }
 
        // 判断是否有socket数据达到:即socket就绪
        if(FD_ISSET(client_fd, &set))
        {
            char buf[60] = {0};
            //从socket的缓冲区中, 读取到达数据
            int res_recv = recv(client_fd, buf, sizeof(buf), 0);
            //如果recv返回值为0, 表示对方断开链接
            if(res_recv == 0)
            {
                printf("-----服务器断开连接 或 被服务端踢出连接------");
                break;
            }
            // 打印读取的数据到控制台
            printf("msg: %s \n", buf);
        }
    }
    close(client_fd);
    return 0;
}
