#include "../kernel/types.h"
#include "user.h"

int
main(int argc, char *argv[]) {
    if (argc <= 1 || argc > 2) {
        fprintf(2, "sleep: only one argument is allowed\n");
        exit(1);
    }

    int sleep_time = atoi(argv[1]);
    sleep(sleep_time);
    exit(0);
}