#include <my_header.h>

/*

*/
int main(int argc, char* argv[]){                                  
    for(int i=0; i<3; i++){
        printf("a\n");
        fork();
    }

    return 0;
}
