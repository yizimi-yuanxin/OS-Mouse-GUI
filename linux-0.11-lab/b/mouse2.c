#define __LIBRARY__
#include <stdio.h>
#include <unistd.h>
#include <time.h>
__always_inline _syscall0(int,get_message)

int main() {
    int m, i, j;
    for (i = 0; i < 110; ++i)
        m = get_message(),
        printf("message: %d\n", m),
        sleep(10);
    return 0;
}