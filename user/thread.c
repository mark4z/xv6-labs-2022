#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int
main(int argc, char *argv[]) {
    char c;

    if (fork() == 0) {
        c = '\\';
    } else {
        c = '/';
    }

    for (int i = 0;; ++i) {
        if (i % 1000 == 0) {
            write(2, &c, 1);
        }
    }
    exit(0);
}
