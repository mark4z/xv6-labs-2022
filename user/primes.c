#include <stdint.h>
#include "kernel/types.h"
#include "user/user.h"

void primes(int);

int
main(int argc, char *argv[]) {
    int fd[2];
    pipe(fd);
    int rc = fork();
    if (rc < 0) {
        exit(1);
    } else if (rc == 0) {
        close(fd[1]);
        primes(fd[0]);
        exit(0);
    } else {
        close(fd[0]);
        for (int i = 2; i < 36; ++i) {
            write(fd[1], &i, sizeof(int));
        }
        close(fd[1]);
        wait(0);

        exit(0);
    }
}


void primes(int in) {
    int num;
    if (read(in, &num, sizeof(num)) == 0) {
        return;
    }
    fprintf(1, "prime %d\n", num);

    int fd[2];
    pipe(fd);

    int rc = fork();
    if (rc < 0) {
        exit(1);
    } else if (rc == 0) {
        close(fd[1]);
        primes(fd[0]);
    } else {
        close(fd[0]);
        int eof;
        int pass;
        do {
            eof = read(in, &pass, sizeof(pass));
            if (pass % num != 0) {
                write(fd[1], &pass, sizeof(int));
            }
        } while (eof);
        close(fd[1]);
    }
    wait(0);
}