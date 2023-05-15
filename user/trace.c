#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(2, "trace [mask] [command]");
        exit(1);
    }
    trace(atoi(argv[1]));

    char *newargs[MAXARG];
    for (int i = 2; i < argc; ++i) {
        newargs[i - 2] = malloc(strlen(argv[i]) + 1);
        strcpy(newargs[i - 2], argv[i]);
    }
    exec(newargs[0], newargs);
    exit(0);
}