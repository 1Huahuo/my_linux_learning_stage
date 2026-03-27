#include <my_header.h>

int main(int argc, char *argv[]){

    if(1)
        ARGS_CHECK(argc, 3);
    else
        printf("hello world!");


    return 0;
}

