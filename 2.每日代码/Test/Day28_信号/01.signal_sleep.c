#include <my_header.h>

void fun1(int sig_val){
    printf("1111111111111111111\n");
}

void fun2(int sig_val){
    printf("2222222222222222222\n");
}

int main(int argc, char* argv[]){                                  
   signal(SIGINT, fun1);

   unsigned int ret_time1 = sleep(10);
   printf("ret_time1 = %u\n", ret_time1);

   signal(SIGINT, fun2);
   unsigned int ret_time2 = sleep(10);
   printf("ret_time2 = %u\n", ret_time2);

    return 0;
}

