#include <my_header.h>
#include <stdio.h>

int main(int argc, char *argv[]){

    // ./test8_chmod 666 111.txt

    // argv[1] ===== 666
    // argv[2] ===== 111.txt
    
    mode_t mode;
    sscanf(argv[1], "%o", &mode);

    int ret = chmod(argv[2], mode);
    ERROR_CHECK(ret, -1, "chmod failed");

    return 0;
}

