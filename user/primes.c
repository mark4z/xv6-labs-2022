#include <stdint.h>
#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[]) {
    int fd[2];
    pipe(fd);

    int child = fork();
    if (child == 0) {
        int num;
        while (read(fd[0], &num, sizeof(num)) != 0) {
            fprintf(1, "prime %d\n", num);
        }
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