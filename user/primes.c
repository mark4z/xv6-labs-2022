#include <stdint.h>
#include "kernel/types.h"
#include "user/user.h"

void primes();

int
main(int argc, char *argv[]) {
    int fd[2];
    pipe(fd);

    int child = fork();
    if (child == 0) {
        dup(fd[0]);
        dup(fd[1]);

        close(fd[1]);
        close(fd[0]);

        int num;
        if (read(4, &num, sizeof(num)) != 0) {
            fprintf(1, "prime %d\n", num);
        }
        while (read(4, &num, sizeof(num)) != 0) {
            write(5, &num, sizeof(int));
        }
        primes();
        exit(0);
    } else {
        close(fd[0]);
        for (int i = 2; i < 35; ++i) {
            write(fd[1], &i, sizeof(int));
        }
        close(fd[1]);
        wait(0);
    }


    exit(0);
}


void primes() {
    int child = fork();
    if (child == 0) {
        dup(0);
        dup(1);
        close(0);
        close(1);
        dup(5);
        dup(4);
        close(4);
        close(5);
        int num;
        if (read(0, &num, sizeof(num)) != 0) {
            fprintf(1, "prime %d\n", num);
        } else {
            exit(0);
        }
        while (read(0, &num, sizeof(num)) != 0) {
            write(1, &num, sizeof(int));
        }
        primes();
        close(0);
        close(1);
        exit(0);
    }
    wait(0);
}