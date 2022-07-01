#define __LIBRARY__
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
/*
typedef struct {
	int xpos1, xpos2;
	int ypos1, ypos2;
} paint_pos;
typedef struct paint_pos paint_pos;
*/
_syscall1(int, get_message, int*, msg)
_syscall0(int, init_graphics)
_syscall2(int, repaint, (paint_pos), pos, char, x)
_syscall2(int, timer_create, int, millseconds, int, type)

#define BIRD_X      120
#define BIRD_WIDTH  10
#define BIRD_HEIGHT 8

typedef struct object object;
struct object {
    int posx, posy, width, height;
};

paint_pos ob[20];
paint_pos bird, all;

#define MAX_X 320
#define MAX_Y 200
#define FLUSH_TIME 1

struct object objects[20];

int bird_y = 100;
int minn(int a, int b) { return a < b ? a : b; }

int object_count;
int paint_ob(int if_init) {
    int i, j, k;
    for (i = 0; i < object_count; ++i) {
        if (!if_init)
            repaint(ob[i], 3);

        if (objects[i].posx + 10 < 0) continue;
        ob[i].xpos1 = maxx(0, objects[i].posx);
        ob[i].xpos2 = minn(MAX_X, objects[i].posx + 10);
        ob[i].ypos1 = (objects[i].posy == 0) ? MAX_Y - objects[i].height : 0;
        ob[i].ypos2 = (objects[i].posy == 0) ? MAX_Y : objects[i].height;

        repaint(ob[i], 12);
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
    repaint(all, 44);
}

int main() {
    int pos, i, j, *m;
    int now_x = 50, now_y = 100;
    int ok = 0;
    srand((unsigned)time(NULL));
    object_count = rand() % 20 + 1;
    for (i = 0; i < object_count; ++i) {
        objects[i].posx = 200 + i * 50;
        objects[i].posy = rand() % 2;
        objects[i].height = (rand() % 10) * 10;
        objects[i].width = 10;
        ob[i].xpos1 = objects[i].posx;
        ob[i].xpos2 = objects[i].posx + 10;
        ob[i].ypos1 = (objects[i].posy == 0) ? MAX_Y - objects[i].height : 0;
        ob[i].ypos2 = (objects[i].posy == 0) ? MAX_Y : objects[i].height;
    }
    init_graphics();

    all.xpos1 = 0, all.xpos2 = 319;
    all.ypos1 = 0, all.ypos2 = 199;
    
    repaint(all, 3);

    bird.xpos1 = now_x - (BIRD_WIDTH / 2);
    bird.xpos2 = now_x + (BIRD_WIDTH / 2);
    bird.ypos1 = now_y - (BIRD_HEIGHT / 2);
    bird.ypos1 = now_y + (BIRD_HEIGHT / 2);

    repaint(bird, 12);

    paint_ob(1);
    /*
    */
    timer_create(4000, 0);
    while(1) {
        get_message(m);
        if (*m == 0) continue;
        printf("message: %d\n", *m);
        if (*m == 1)  /* 经测试不知道为啥有时候会返回0有时候会返回1 */
            now_y -= 20;
        if (*m == 2)
            now_y += 20;
        repaint(bird, 3);
        bird.ypos1 = now_y - (BIRD_HEIGHT / 2);
        bird.ypos2 = now_y + (BIRD_HEIGHT / 2);
        repaint(bird, 12);
        /*paint_bd(now_x, now_y);
        
        */
        if (get_stucked(now_x, now_y)) { ok = 1; break; }
        if (now_y - (BIRD_HEIGHT / 2) < 0 || now_y + (BIRD_HEIGHT / 2) >= 200) { ok = 1; break; }
        
        paint_ob(0);
    }
    if (ok) _GAME_OVER();
    return 0;
}