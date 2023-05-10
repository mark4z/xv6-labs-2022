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
        exit(1);
    } else if (rc == 0) {
        primes();
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
    close(4);
    int in = 3;

    int num;
    if (read(in, &num, sizeof(num)) == 0) {
        return;
    }
    fprintf(1, "prime %d\n", num);

    int fd[2];
    pipe(fd);
    int out = fd[1];

    int rc = fork();
    if (rc < 0) {
        return;
    } else if (rc == 0) {
        close(5);
        close(3);
        dup(4);
        primes();
        exit(0);
    } else {
        int pass;
        int eof;
        do {
            eof = read(in, &pass, sizeof(pass));
            if (pass % num != 0) {
                write(out, &pass, sizeof(int));
            }
        } while (eof);

        close(in);
        close(out);

        wait(0);
        exit(0);
    }
}

