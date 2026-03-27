#include <my_header.h>

#define BUF_SIZE 1024

int pipe_ab[2];
int pipe_ba[2];

pid_t pid;

void handle_sigusr1(int sig_val) {
    printf("\nProcess %d received SIGUSR1, exiting...\n", getpid());
    close(pipe_ab[0]);
    close(pipe_ab[1]);
    close(pipe_ba[0]);
    close(pipe_ba[1]);
    exit(0);
}

int main() {
    char buf[BUF_SIZE];

    signal(SIGUSR1, handle_sigusr1);

    if (pipe(pipe_ab) == -1 || pipe(pipe_ba) == -1) {
        perror("pipe");
        exit(1);
    }

    pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        while (1) {
            if (fgets(buf, BUF_SIZE, stdin) != NULL) {
                if (buf[0] == '1') {
                    kill(getppid(), SIGUSR1);
                    kill(getpid(), SIGUSR1);
                }

                write(pipe_ba[1], buf, strlen(buf) + 1);
            }

            int n = read(pipe_ab[0], buf, BUF_SIZE);
            if (n > 0) {
                printf("B received: %s", buf);
            }
        }
    }

    else {
        while (1) {
            if (fgets(buf, BUF_SIZE, stdin) != NULL) {
                if (buf[0] == '1') {
                    kill(pid, SIGUSR1);
                    kill(getpid(), SIGUSR1);
                }

                write(pipe_ab[1], buf, strlen(buf) + 1);
            }

            int n = read(pipe_ba[0], buf, BUF_SIZE);
            if (n > 0) {
                printf("A received: %s", buf);
            }
        }
    }

    return 0;
}
