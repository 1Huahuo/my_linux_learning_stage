#include <my_header.h>
#include "separate_cmd_path.h"
#include "cmd_set.h"
#include "hash.h"

char cur_path[1024] = "/"; // 当前工作目录，默认为"/"

int main(int argc, char* argv[]){                                  
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(client_fd, -1, "socket");

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("192.168.85.128");
    server_addr.sin_port = htons(atoi("12345"));

    int ret_connect = connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    ERROR_CHECK(ret_connect, -1, "connect");

    char buf[256] = {0};
    // -------------------注册与登录-------------------
    char logged_in = 0;
    char cur_user[21] = {0};
    while (!logged_in){
        printf("Enter login/register user_name: ");
        fgets(buf, sizeof(buf), stdin);

        CmdType cmd_type;
        char username[256] = {0};
        // 用已有的get_cmd和get_path分离出命令类型与用户名
        get_cmd(buf, &cmd_type);
        get_path(buf, username);

        if (cmd_type != CMD_LOGIN && cmd_type != CMD_REGISTER) {
            printf("must login or register\n");
            continue;
        }

        // 发送命令类型和用户名
        send(client_fd, &cmd_type, sizeof(cmd_type), MSG_NOSIGNAL);
        send(client_fd, username, sizeof(username), MSG_NOSIGNAL);

        // 提示输入密码
        char password[256];
        printf("password: ");
        fgets(password, sizeof(password), stdin);
        password[strcspn(password, "\n")] = '\0'; // strcspn:查找字符串开头连续不含指定字符集的最大长度

        // 发送密码
        send(client_fd, password, sizeof(password), MSG_NOSIGNAL);

        // 接收服务端响应
        char response[256] = {0};
        recv(client_fd, response, sizeof(response)-1, 0);
        printf("%s\n", response); // 服务端会回应是否注册/登录成功

        if (strstr(response, "success") != NULL) { // strstr:在字符串中查找子串首次出现的位置,比strcmp更加宽松
            logged_in = 1;
            strcpy(cur_user, username);
        }
    }
    

    // -------------------客户端请求-------------------
    while(1){
        printf("%s:%s$ ", cur_user, cur_path);
        bzero(buf, sizeof(buf));
        fgets(buf, sizeof(buf), stdin);

        // 获取命令类型
        CmdType cmd_type;
        get_cmd(buf, &cmd_type);
        if(cmd_type == CMD_UNKNOW){
            printf("The command is invalid.\n");
            exit(1);
        }
        // 将命令类型发送给服务端
        send(client_fd, &cmd_type, sizeof(cmd_type), MSG_NOSIGNAL);

        // 获取路径
        char path[256] = {0};
        // 将路径发送给服务端
        if (cmd_type != CMD_PWD) {
            get_path(buf, path);
            send(client_fd, path, sizeof(path), MSG_NOSIGNAL); // 纯路径+全'\0'，必须传256个字节，因为服务端是MSG_WAITALL标志
        }

        // 然后根据命令类型执行相应的处理逻辑
        switch(cmd_type){
        case CMD_CD:
            cmd_cd(client_fd);
            break;
        case CMD_LS:
            cmd_ls(client_fd);
            break;
        case CMD_PUTS:
            cmd_puts(client_fd, path);
            break;
        case CMD_GETS:
            cmd_gets(client_fd, path);
            break;
        case CMD_REMOVE:
            cmd_remove(client_fd);
            break;
        case CMD_PWD:
            cmd_pwd(client_fd);
            break;
        case CMD_MKDIR:
            cmd_mkdir(client_fd);
            break;
        case CMD_RMDIR:
            cmd_rmdir(client_fd);
            break;
        default:
            break;
        }
    }

    close(client_fd);
    return 0;
}
