#include <stdint.h>
#include "kernel/types.h"
#include "user/user.h"

void primes();

int
main(int argc, char *argv[]) {
    int fd[2];
    pipe(fd);
    int rc = fork();
    if (rc < 0) {
        return 1;
    } else if (rc == 0) {
        close(fd[1]);
        int num;
        int in = fd[0];
        if (read(in, &num, sizeof(num)) == 0) {
            exit(0);
        }
        fprintf(1, "prime %d\n", num);

        int fdNew[2];
        pipe(fdNew);
        int nextIn = fdNew[0];
        int out = fdNew[1];

        close(0);
        dup(nextIn);
        primes();
        close(0);
        close(nextIn);

        int pass;
        while (read(in, &pass, sizeof(pass)) != 0) {
            if (pass % num != 0) {
                write(out, &pass, sizeof(int));
            } else {
            }
        }
        close(out);
        close(in);
        wait(0);

        exit(0);
    } else {
        close(0);
        close(fd[0]);
        for (int i = 2; i < 35; ++i) {
            write(fd[1], &i, sizeof(int));
        }
        close(fd[1]);
        wait(0);

        exit(0);
    }
}


void primes() {
    int child = fork();
    if (child < 0) {
        exit(1);
    } else if (child == 0) {
        close(3);
        dup(0);
        int in = 3;

        int num;
        if (read(in, &num, sizeof(num)) == 0) {
            exit(0);
        }
        fprintf(1, "prime %d\n", num);

        int fd[2];
        pipe(fd);
        int newIn = fd[0];
        int out = fd[1];

        close(0);
        dup(newIn);
        primes();
        close(0);

        close(newIn);
        int pass;
        while (read(in, &pass, sizeof(pass)) != 0) {
            if (pass % num != 0) {
                write(out, &pass, sizeof(int));
            }
        }
        close(in);
        close(out);
        wait(0);

        exit(0);
    }
}