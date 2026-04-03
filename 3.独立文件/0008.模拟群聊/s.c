#include <my_header.h>

// 聊天室服务端
typedef struct conn_s
{
    int netfd;
    int isalive;
} conn_t;

int main(int argc,char* argv[])
{
    //启动socket函数, 获得对应的socket_fd描述符
    int sofd = socket(AF_INET, SOCK_STREAM, 0);

    //创建服务端的地址和端口
    struct sockaddr_in sockaddr;
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = inet_addr("192.168.85.128");
    sockaddr.sin_port = htons(atoi("12345"));

    //绑定端口ip
    bind(sofd, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
    //监听端口
    listen(sofd, 10);

    //构建select监听的集合: fd_set类型
    fd_set set;
    //清空set: 初始化set
    FD_ZERO(&set);
    FD_SET(sofd, &set);

    //客户端数组: 用于保存所有链接服务器的客户端
    conn_t list[1024];
    memset(list, 0, sizeof(list));
    //客户端的存在个数
    int index = 0;
    
    while(1)
    {
        //构建此次的监听集合
        fd_set temp_set;
        // 把set记录复制到本次监听集合
        memcpy(&temp_set, &set, sizeof(set));
        
        // select开始监听有没有就绪事件
        select(10, &temp_set, NULL, NULL, NULL);
        
        // 判断是否有socket数据达到:即socket就绪
        if(FD_ISSET(sofd, &temp_set))
        {
            //获得一个新的客户端链接
            int netfd = accept(sofd, NULL, NULL);
            //保存到""客户端""链接数组中
            list[index].isalive = 1;
            list[index].netfd = netfd;
            // 下一次增加监听这个客户端
            FD_SET(netfd, &set);
            
            index++;
        }
  
        //遍历客户端列表 
        for(int i=0; i<index; i++)
        {
            //当前遍历的客户端
            conn_t con = list[i];
            //判断这个客户端是否存活, 以及是否就绪: 即是否有消息到来
            if(con.isalive == 1 && FD_ISSET(con.netfd, &temp_set))
            {
                //该客户端存活, 且处于就绪状态
                //读取客户端的到达信息
                char buf[60] = {0};
                int res_recv = recv(con.netfd, buf, sizeof(buf), 0);
                // 如果返回值为0, 说明这个客户端已经断开
                if(res_recv == 0)
                {
                    //把客户端数组中该标记置为0: 即非存活状态
                    list[i].isalive = 0;
                    //取消后续对次客户端的select监控
                    FD_CLR(con.netfd, &set);
                    // 关闭这个客户端
                    close(list[i].netfd);
                }
                else
                {
                    //把读取到的数据, 分发给别的客户端
                    //遍历所有客户端
                    for(int j=0; j<index; j++)
                    {
                        if(list[j].isalive == 0|| j == i)
                        {
                            //跳过断开链接的客户端, 跳过发信息过来的客户端
                            continue;
                        }
                        //发送信息
                        send(list[j].netfd, buf, sizeof(buf), 0);
                    }
                }
            }
        }
    }
    
    close(sofd);
    return 0;
}
