#define __LIBRARY__
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

_syscall0(int, get_message)
_syscall0(int, init_graphics)
_syscall3(int, repaint, int, xpos, int, ypos, char, x)

#define BIRD_X      120
#define BIRD_WIDTH  10
#define BIRD_HEIGHT 8
typedef struct object object;
struct object {
    int posx, posy, width, height;
};

#define MAX_X 320
#define MAX_Y 200
#define FLUSH_TIME 1

struct object objects[20];

int bird_y = 100;
int minn(int a, int b) { return a < b ? a : b; }

int paint_bd(int now_x, int now_y) {
    int i, j;
    for (i = now_x - (BIRD_WIDTH / 2); i <= now_x + (BIRD_WIDTH / 2); ++i) {
        for (j = now_y - (BIRD_HEIGHT / 2); j <= now_y + (BIRD_HEIGHT / 2); ++j) {
            int res = repaint(i, j, 12);
        }
    }
}

int object_count;
int paint_ob() {
    int i, j, k;
    for (i = 0; i < object_count && objects[i].posx < MAX_X; ++i) {
        for (j = objects[i].posx; j < minn(objects[i].posx + 10, MAX_X); ++j) {
            if (objects[i].posy == 0) {
                for (k = MAX_Y - objects[i].height; k < MAX_Y; ++k) {
                    int res = repaint(j, k, 12);
                }
            } else {
                for (k = 0; k < objects[i].height; ++k) {
                    int res = repaint(j, k, 12);
                }
            }
        }
    }
}

int init_gra() {
    int i, j;
    for (i = 0; i < MAX_X; ++i) {
        for (j = 0; j < MAX_Y; ++j) {
            int res = repaint(i, j, 3);
        }
    }
}

int get_stucked(int posx, int posy) {
    int pos, i, j;
    for (pos = 0; pos < object_count; ++pos) {
        if (posx - (BIRD_WIDTH / 2) > objects[pos].posx + 10) break;
        if (posx + (BIRD_WIDTH / 2) < objects[pos].posx) continue;
        if ((posx - (BIRD_WIDTH / 2) >= objects[pos].posx && posx - (BIRD_WIDTH / 2) <= objects[pos].posx + 10)
         || (posx + (BIRD_WIDTH / 2) >= objects[pos].posx && posx + (BIRD_WIDTH / 2) <= objects[pos].posx + 10)
         || (posx - (BIRD_WIDTH / 2) >= objects[pos].posx && posx + (BIRD_WIDTH / 2) <= objects[pos].posx + 10)) 
        {
            if ((objects[pos].posy == 0 && MAX_Y - objects[pos].height <= posy + (BIRD_HEIGHT / 2))
             || (objects[pos].posy == 1 &&         objects[pos].height >= posy - (BIRD_HEIGHT / 2))) 
            {
                return 1;
            }
        }
    }
    return 0;
}

int _GAME_OVER() {
    int i, j;
    for (i = 0; i < MAX_X; ++i) {
        for (j = 0; j < MAX_Y; ++j) {
            int res = repaint(i, j, 44);
        }
    }
}

int main() {
    srand((unsigned)time(NULL));
    int pos, i, j, m;
    int now_x = 50, now_y = 100;
    int ok = 0;
    init_graphics();
    object_count = rand() % 20 + 1;
    for (i = 0; i < object_count; ++i) {
        objects[i].posx = 200 + i * 50;
        objects[i].posy = rand() % 2;
        objects[i].height = (rand() % 10) * 10;
        objects[i].width = 10;
    }
    init_gra();
    paint_bd(now_x, now_y);
    paint_ob();
    for (pos = 0; pos < 100; ++pos) {
        init_gra();
        paint_ob();
        paint_bd(now_x, now_y);
        m = get_message();
        now_y = 100 - m * 20;
        if (get_stucked(now_x, now_y)) { ok = 1; break; }
        if (now_y - (BIRD_HEIGHT / 2) < 0 || now_y + (BIRD_HEIGHT / 2) >= 200) { ok = 1; break; }
        for (i = 0; i < object_count; ++i)
            objects[i].posx -= 20;
        sleep(FLUSH_TIME);
    }
    if (ok) _GAME_OVER();
    return 0;
}