#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

void xargs(char *input, int argc, char *argv[]) {
    int rc = fork();
    if (rc < 0) {
        fprintf(2, "fail", 4);
        exit(1);
    } else if (rc == 0) {
        int idx = argc - 1;
        char *p = input;
        int token = 0;

        while (*p != '\0') {
            if (*(p + token) == ' ' || *(p + token) == '\0') {
                argv[idx] = malloc(token + 1);
                memcpy(argv[idx], p, token);
                p += token;
                if (*(p) != '\0') {
                    p++;
                }
                token = 0;
                idx++;
            } else {
                token++;
            }
        }
        argv[idx] = 0;

        exec(argv[0], argv);
        exit(1);
    }
    wait(0);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        exit(1);
    }
    char *argvexec[MAXARG];
    for (int i = 1; i < argc; ++i) {
        argvexec[i - 1] = malloc(strlen(argv[i]) + 1);
        strcpy(argvexec[i - 1], argv[i]);
    }

    char buf[512];
    char *p = buf;

    int eof;
    do {
        eof = read(0, p, 1);
        if (*p == '\n') {
            *p = '\0';
            xargs(buf, argc, argvexec);
            buf[0] = '\0';
            p = buf;
        } else {
            p++;
        }
    } while (eof);
    exit(0);
}

