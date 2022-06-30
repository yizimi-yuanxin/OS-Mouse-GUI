#define __LIBRARY__
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

_syscall0(int, get_message)
_syscall0(int, init_graphics)
_syscall3(int, repaint, int, xpos, int, ypos, char, x)
_syscall0(int, get_mouse_posx)
_syscall0(int, get_mouse_posy)

#define MAX_X           320
#define MAX_Y           200

#define MOUSE_WIDTH     10
#define MOUSE_HEIGHT    8

void paint_bg() {
    int i, j;
    for (i = 0; i < MAX_X; ++i) {
        for (j = 0; j < MAX_Y; ++j) {
            int ret = repaint(i, j, 3);
        }
    }
}

int maxx(int a, int b) { return a > b ? a : b; }
int minn(int a, int b) { return a < b ? a : b; }

void paint_ms(int posx, int posy) {
    int i, j;
    for (i = maxx(0, posx - (MOUSE_WIDTH / 2)); i < minn(MAX_X, posx + (MOUSE_WIDTH / 2)); ++i) {
        for (j = maxx(0, posy - (MOUSE_HEIGHT / 2)); j < minn(MAX_Y, posy + (MOUSE_HEIGHT / 2)); ++j) {
            int ret = repaint(i, j, 12);
        }
    }
}

int main() {
    int nowx, nowy;
    int cnt = 0;
    init_graphics();

    while(1) {
        if (cnt >= 1000) { 
            cnt = 0;
            paint_bg();
        }
        nowx = get_mouse_posx();
        nowy = get_mouse_posy();
        paint_ms(nowx, nowy);
        ++cnt;
    }
    return 0;
}