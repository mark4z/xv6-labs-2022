#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

char *
fmtname(char *path) {
    static char buf[DIRSIZ + 1];
    char *p;

    // Find first character after last slash.
    for (p = path + strlen(path); p >= path && *p != '/'; p--);
    p++;

    // Return blank-padded name.
    if (strlen(p) >= DIRSIZ)
        return p;
    memmove(buf, p, strlen(p));
    buf[strlen(p)] = 0;
    return buf;
}

void find(char *path, char *file) {
    int fd;
    struct stat st;
    struct dirent de;
    char buf[512], *p;

    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "path not exists.\n");
        return;
    }
    if (fstat(fd, &st) < 0) {
        fprintf(2, "path can not stat.\n");
        close(fd);
        return;
    }
    switch (st.type) {
        case T_DEVICE:
            break;
        case T_FILE:
            char *fileName = fmtname(path);
            if (strcmp(fileName, file) == 0) {
                fprintf(1, "%s\n", path);
            }
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
                if (de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) {
                    continue;
                }
                memmove(p, de.name, strlen(de.name));
                p[strlen(de.name)] = 0;

                if (stat(buf, &st) < 0) {
                    fprintf(2, "can not stat %s\n", buf);
                    continue;
                }
                find(buf, file);
                *p = '\0';
            }
            break;
    }
    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(2, "find [path] [fileName] etc.\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}

