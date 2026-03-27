#include <my_header.h>

/*

*/
int main(int argc, char* argv[]){                                  
    for(int i = 0; i < 3; i++){
        fork();
        printf("a"); // 以此来证明用户区缓冲区也会在fork时被复制
    }
    printf("\n");
    /* fflush(stdout); */

    return 0;
}

