#include <my_header.h>
#define BUF_SIZE 1024

int fd_write, fd_read;

void fun(int sig_val){
    printf("--------------------\n");
    close(fd_write);
    close(fd_read);
    exit(0);
}

int main(int argc, char* argv[]){
    fd_read = open("1.pipe", O_RDONLY);
    ERROR_CHECK(fd_read, -1, "open 1.pipe");
    fd_write = open("2.pipe", O_WRONLY);
    ERROR_CHECK(fd_write, -1, "open 2.pipe");

    signal(SIGUSR1, fun);

    pid_t pid = getpid();
    write(fd_write, &pid, sizeof(pid));
    read(fd_read, &pid, sizeof(pid));

    printf("my pid:%d, a pid:%d\n", getpid(), pid);

    /* sleep(5); */
    /* kill(getpid(), SIGUSR1); */
    /* kill(pid, SIGUSR1); */
    while(1){
        char buf[BUF_SIZE];
        memset(buf, 0, BUF_SIZE);
        int count;
        if((count = read(STDIN_FILENO, buf, BUF_SIZE)) > 0){
            if(strcmp(buf, "1\n") == 0){
                kill(pid, SIGUSR1);
                kill(getpid(), SIGUSR1);
            }
            write(fd_write, buf, count - 1);
        }
        int n;
        memset(buf, 0, BUF_SIZE);
        if((n = read(fd_read, buf, BUF_SIZE)) > 0)
            printf("b receive:%s\n", buf);
    }

    return 0;
}
