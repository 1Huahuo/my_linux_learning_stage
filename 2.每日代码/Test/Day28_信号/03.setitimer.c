#include <my_header.h>

void fun(int sig_val){
    printf("----%d----\n", sig_val);
}

int main(int argc, char* argv[]){                                  
    signal(SIGALRM, fun);
    struct itimerval timer;
    memset(&timer, 0, sizeof(timer));

    timer.it_value.tv_sec = 10;
    timer.it_value.tv_usec = 0;
    
    timer.it_interval.tv_sec = 2;
    timer.it_interval.tv_usec = 0;

    setitimer(ITIMER_REAL, &timer, NULL);

    while(1);

    return 0;
}

