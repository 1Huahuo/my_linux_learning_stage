#include <my_header.h>

void fun(int sig_val){
    printf("signal 2 handler begin, input signal 3\n");
    sleep(5);
    sigset_t set;
    sigemptyset(&set);
    sigpending(&set);
    if(sigismember(&set, 3))
        printf("pending:3 exist\n");
    else
        printf("pending:3 not exist\n");
    printf("sigval = %d\n", sig_val);
    printf("signal 2 handler over\n");
}

int main(int argc, char* argv[]){                                  
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = fun;
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, 3);
    act.sa_mask = set;
    act.sa_flags = SA_RESTART;

    sigaction(2, &act, NULL);
    printf("sleep begin, input signal 2\n");
    sleep(5);
    printf("sleep over\n");
    
    printf("before while\n");
    while(1);
    printf("after while\n");

    return 0;
}

