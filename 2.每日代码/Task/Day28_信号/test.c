#include <my_header.h>
#define BUF_SIZE 1024

int fd_read;

void fun(int sig_val){
    close(fd_read);
    exit(0);
}

int main(int argc, char* argv[]){          
    fd_read = open("test.txt", O_RDONLY);

    signal(SIGUSR1, fun);
    char buf[BUF_SIZE];
    memset(buf, 0, BUF_SIZE);
    int count;
    sleep(10);
    while((count = read(fd_read, buf,BUF_SIZE)) >0)
        if(count == 0)
            kill(getpid(), SIGUSR1);
 
 
    return 0;
}
