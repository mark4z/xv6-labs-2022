#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

extern int sof_r_sp(void);

__attribute__((noreturn))
void dfs(int level) {
    printf("%d\n", level);
    if (level == 257) {
        exit(0);
    }
    dfs(level + 1);
}

int main() {
    int level = 0;
    printf("%d\n", level);
    dfs(level);
    exit(0);
}

