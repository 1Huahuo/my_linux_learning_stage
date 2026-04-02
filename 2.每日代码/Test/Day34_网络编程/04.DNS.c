#include <my_header.h>

// 通过getaddrinfo: 把域名变成ip
int main(int argc, char* argv[]){
    ARGS_CHECK(argc, 2);

   struct addrinfo *res;
   // DNS请求,获得结果
   /* getaddrinfo("cskaoyan.com", NULL, NULL, &res); */
   getaddrinfo(argv[1], NULL, NULL, &res);
   //遍历res  
   for (struct addrinfo *p = res; p != NULL; p = p->ai_next) {  
        struct sockaddr  *ipTag = p->ai_addr;
        if (p->ai_family == AF_INET) { // 判断获得的是否是IPv4地址
            // 类型转换
            struct sockaddr_in *socketTagV4 = (struct sockaddr_in *) ipTag;
            // 获取IP地址
            struct in_addr ipTagV4 = socketTagV4->sin_addr;
            // 把网路IP转成点分十进制字符串
            char * strIp = inet_ntoa(ipTagV4);
            printf("IPv4,  ip = %s  \n",  strIp);
        }
   }  
   // 释放
   freeaddrinfo(res);
   return 0;
}
