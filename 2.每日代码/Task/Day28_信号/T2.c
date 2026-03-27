#include <my_header.h>

int count = 0;

void fun(int sig_val){
    if(count == 0){
        printf("before first sleep\n");
        sleep(10);
        printf("after first sleep\n");
    }
    count++;
    printf("signal of %d appeared counts = %d\n",sig_val, count );
}

int main(int argc, char* argv[]){                                  
    signal(SIGINT, fun);

    
    printf("before while, all handler over\n");
    while(1);
    printf("after while\n");
    return 0;
}

