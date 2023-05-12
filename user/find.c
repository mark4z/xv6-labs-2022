#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

void find(char *path) {
    int fd;
    struct stat st;
    struct dirent de;
    char buf[1024], *p;

    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "path not exists.\n");
        return;
    }
    if (stat(path, &st) < 0) {
        fprintf(2, "path can not stat.\n");
        close(fd);
        return;
    }
    switch (st.type) {
        case T_DEVICE:
            break;
        case T_FILE:
            fprintf(1, "%s\n", path);
            break;
        case T_DIR:
            if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
                fprintf(2, "path too long\n");
                break;
            }
            strcpy(buf, path);
            p = buf + strlen(buf);
            *p++ = '/';

            while ((read(fd, &de, sizeof de)) == sizeof de) {
                if (de.inum == 0) {
                    continue;
                }
                if (stat(buf, &st) < 0) {
                    fprintf(2, "can not stat %s\n", buf);
                    continue;
                }
                if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
                    fprintf(2, "path too long\n");
                    break;
                }
                memmove(p, de.name, strlen(de.name));
                p[strlen(de.name)] = 0;
                if (strcmp(de.name, ".") != 0 && strcmp(de.name, "..") != 0) {
                    find(buf);
                }
                *p = '\0';
            }
            break;
    }
    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(2, "input one folder at lease pls.\n");
        exit(1);
    }
    for (int i = 1; i < argc; ++i) {
        find(argv[i]);
    }
    exit(0);
}

