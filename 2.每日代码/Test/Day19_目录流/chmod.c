#include <my_header.h>
#include <stdio.h>

int main(int argc, char *argv[]){

    int ret = chmod("1.txt", 0777);

    printf("ret = %d\n", ret);
    perror("chmod 1.txt");

    printf("66666666666.\n");


    return 0;
}

