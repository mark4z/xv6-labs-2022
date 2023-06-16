#include <printf.h>

struct bucket_entry {
    int a;
};

struct bucket_entry arr[1];

int
main(int argc, char *argv[]) {
    arr[0].a = 1;
    printf("%d", arr[0].a);
}