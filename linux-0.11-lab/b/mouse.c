#define __LIBRARY__
#include <stdio.h>
#include <unistd.h>
#include <time.h>
_syscall1(int,get_message, int*, msg)
_syscall2(int,timer_create,int,millsoconds,int,type)

int main() {
    int m, i, j;
    timer_create(2000, 0);
    for (i = 0; i > -1; ++i) {
        get_message(&m);
        if (m != -1)
            printf("message: %d\n", m);
    }
        /* sleep(10); */
    return 0;
}