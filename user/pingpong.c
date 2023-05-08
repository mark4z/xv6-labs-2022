#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[]) {
    int fd[2];
    pipe(fd);
    int child = fork();
    if (child == 0) {
        char buf[1];
        read(fd[1], buf, 1);
        fprintf(1, "%d: received ping\n", getpid());
        exit(0);
    } else {
        write(fd[0], "1", 1);
        wait(0);
        fprintf(1, "%d: received pong\n", getpid());
    }
    close(fd[0]);
    close(fd[1]);
    exit(0);
}