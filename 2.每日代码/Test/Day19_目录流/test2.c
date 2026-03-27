#include <my_header.h>
#include <stdio.h>

int main(int argc, char *argv[]){

    // sscanf ：输入。 从字符串输入。 
    // ./test2 111 helloworldhello

    char *p = argv[1];

    int num1 = 0;
    sscanf(p, "%d", &num1);

    printf("input number = %d----\n", num1);
    return 0;
}

