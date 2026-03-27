#include <my_header.h>
#include <sys/stat.h>

int main(int argc, char *argv[]){

    int ret = chmod("1.txt", 0777);
    ERROR_CHECK(ret, -1, "chmod failed");


    return 0;
}

