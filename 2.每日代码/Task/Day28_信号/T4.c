#include <my_header.h>

int main(int argc, char* argv[]){                                  
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, 2);
    sigprocmask(SIG_BLOCK, &set, NULL);
    
    printf("sleep begin\n");
    sleep(5);
    sigset_t set1;
    sigemptyset(&set1);
    sigpending(&set1);
    if(sigismember(&set1, 2))
        printf("pending:signal 2 exist\n");
    else
        printf("pending:signal 2 not exist\n");
    printf("sleep over\n");

    sigprocmask(SIG_UNBLOCK, &set, NULL);

    return 0;
}

