#include <stdint.h>
#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[]) {
    int fd[2];
    pipe(fd);

    int child = fork();
    if (child == 0) {
        uint32_t num;
        while (read(fd[1], &num, sizeof(num)) != 0) {
            fprintf(1, "prime %d\n", num);
        }
        exit(0);
    } else {
        close(fd[1]);
        for (int i = 2; i < 35; ++i) {
            uint32_t *num = malloc(sizeof(uint32_t));
            *num = i;
            write(fd[0], num, sizeof(num));
        }
        close(fd[0]);
        wait(0);
    }

    exit(0);
}