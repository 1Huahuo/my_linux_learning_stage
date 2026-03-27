#include <my_header.h>

/*

*/

void fun1(int sig_val){
    printf("2222222222222222222\n");
}

void fun2(int sig_val){
    printf("3333333333333333333\n");
}

int main(int argc, char* argv[]){                                  
    signal(SIGINT, fun1);
    signal(SIGQUIT, fun2);
    sigset_t set, oldset;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);

    sigprocmask(SIG_BLOCK, &set, &oldset);
    printf("sleep begin\n");
    sleep(10);
    printf("sleep over\n");
    /* sigprocmask(SIG_UNBLOCK, &set, NULL); */
    /* pause(); */

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGQUIT);
    sigsuspend(&mask);

    printf("before while\n");
    while(1);
    printf("after while\n");

    return 0;
}

