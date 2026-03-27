#include <my_header.h>

/*

*/
int main(int argc, char* argv[]){                                  
    pid_t ret_fork = fork();

    if(ret_fork == 0){
        sleep(5);
        return 0;
    }else{
        for(int i = 0; i < 20; i++){
            sleep(1);
            int status;
            pid_t ret_wait = waitpid(ret_fork, &status, WNOHANG);

            if(ret_wait == ret_fork){
                if(WIFEXITED(status))
                    printf("child status: %d\n", WEXITSTATUS(status));
                else if(WIFSIGNALED(status))
                    printf("child signed: %d\n", WTERMSIG(status));
                break;
            }
        }
    }

    return 0;
}

