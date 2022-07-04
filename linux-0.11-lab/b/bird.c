#define __LIBRARY__
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
typedef struct {
	int xpos1, xpos2;
	int ypos1, ypos2;
} paint_pos_t;
/*
#define DEBUG
*/
/*
typedef struct paint_pos_t paint_pos_t;
*/
_syscall1(int, get_message, int*, msg)
_syscall0(int, init_graphics)
/*
_syscall2(int, repaint, paint_pos*, pos, char, x)
*/
_syscall3(int, repaint, int, xpos, int, ypos, char, x)
_syscall2(int, timer_create, int, millseconds, int, type)

#define BIRD_X      120
#define BIRD_WIDTH  10
#define BIRD_HEIGHT 8

typedef struct {
    int xpos1, xpos2, ypos1, ypos2;
} object;

object ob[20];
object bird, all;

#define MAX_X 320
#define MAX_Y 200
#define FLUSH_TIME 1

object pre[20];
object objects[20];

int bird_y = 100;
int maxx(int a, int b) { return a > b ? a : b; }
int minn(int a, int b) { return a < b ? a : b; }

void paint(object p, char x) {
    int xpos1, xpos2, ypos1, ypos2;
    xpos1 = p.xpos1, xpos2 = p.xpos2;
    ypos1 = p.ypos1, ypos2 = p.ypos2;
    if (xpos1 < 0) xpos1 = 0; 
    if (xpos2 < 0) xpos2 = 0;
    if (ypos1 < 0) ypos1 = 0;
    if (ypos2 < 0) ypos2 = 0;
    if (xpos1 >= MAX_X) xpos1 = MAX_X - 1;
    if (xpos2 >= MAX_X) xpos2 = MAX_X - 1;
    if (ypos1 >= MAX_Y) ypos1 = MAX_Y - 1;
    if (ypos2 >= MAX_Y) ypos2 = MAX_Y - 1;
    if (xpos1 == xpos2 && xpos1 == 0) return;
    if (ypos1 == ypos2 && ypos1 == 0) return;
    if (xpos1 == xpos2 && xpos1 == MAX_X - 1) return;
    if (ypos1 == ypos2 && ypos1 == MAX_Y - 1) return;
#ifdef DEBUG
    printf("repaint: %d %d %d %d %d\n", xpos1, xpos2, ypos1, ypos2, x);
#else
    repaint(((xpos1 << 9) | xpos2), ((ypos1 << 9) | ypos2), x);
#endif // DEBUG
}

int object_count;

int get_stucked(int posx, int posy) {
    int pos, i, j;
    for (pos = 0; pos < object_count; ++pos) {
        if (posx - (BIRD_WIDTH / 2) > objects[pos].xpos2) break;
        if (posx + (BIRD_WIDTH / 2) < objects[pos].xpos1) continue;
        if ((posx - (BIRD_WIDTH / 2) >= objects[pos].xpos1 && posx - (BIRD_WIDTH / 2) <= objects[pos].xpos2)
         || (posx + (BIRD_WIDTH / 2) >= objects[pos].xpos1 && posx + (BIRD_WIDTH / 2) <= objects[pos].xpos2)
         || (posx - (BIRD_WIDTH / 2) >= objects[pos].xpos1 && posx + (BIRD_WIDTH / 2) <= objects[pos].xpos2)) 
        {
            if ((posy - (BIRD_HEIGHT / 2) >= objects[pos].ypos1 && posy - (BIRD_HEIGHT / 2) <= objects[pos].ypos2)
             || (posy + (BIRD_HEIGHT / 2) >= objects[pos].ypos1 && posy + (BIRD_HEIGHT / 2) <= objects[pos].ypos2)
             || (posy - (BIRD_HEIGHT / 2) >= objects[pos].ypos1 && posy + (BIRD_HEIGHT / 2) <= objects[pos].ypos2)) 
            {
                return 1;
            }
        }
    }
    return 0;
}

int _GAME_OVER() {
    /*
    printf("[all]: %4d %4d %4d %4d\n", all.xpos1, all.xpos2, all.ypos1, all.ypos2);
    */
    paint(all, 44);
}

int main() {
    int pos, i, j, *m;
    int now_x = 50, now_y = 100;
    int ok = 0;
    srand((unsigned)time(NULL));
    object_count = rand() % 20 + 1;
    for (i = 0; i < object_count; ++i) {
        objects[i].xpos1 = 200 + i * 50 - (BIRD_WIDTH / 2);
        objects[i].xpos2 = 200 + i * 50 + (BIRD_WIDTH / 2);
        if (rand() % 2)
            objects[i].ypos1 = 0, 
            objects[i].ypos2 = rand() % 100 + 1;
        else 
            objects[i].ypos1 = rand() % 100 + 100,
            objects[i].ypos2 = 199;
    }
    /*
    */
#ifndef DEBUG
    init_graphics();
#endif // !DEBUG

    all.xpos1 = 0, all.xpos2 = 319;
    all.ypos1 = 0, all.ypos2 = 199;

    for (i = 0; i < object_count; ++i)
        paint(objects[i], 12);

    bird.xpos1 = now_x - (BIRD_WIDTH / 2);
    bird.xpos2 = now_x + (BIRD_WIDTH / 2);
    bird.ypos1 = now_y - (BIRD_HEIGHT / 2);
    bird.ypos2 = now_y + (BIRD_HEIGHT / 2);

    paint(bird, 12);

    for (i = 0; i < object_count; ++i) {
        paint(objects[i], 12);
    }
    
    timer_create(2000, 0);
    while(1) {
        get_message(m);
        if (*m == -1) continue;
#ifdef DEBUG
        printf("message: %d\n", *m); 
#endif // DEBUG
        if (*m == 1 || *m == 0) 
            now_y -= 20;
        if (*m == 2)
            now_y += 20;
            
        paint(bird, 3);
        bird.ypos1 = now_y - (BIRD_HEIGHT / 2);
        bird.ypos2 = now_y + (BIRD_HEIGHT / 2);
        paint(bird, 12);

        if (get_stucked(now_x, now_y)) { ok = 1; break; }
        if (now_y - (BIRD_HEIGHT / 2) < 0 || now_y + (BIRD_HEIGHT / 2) >= 200) { ok = 1; break; }
        for (i = 0; i < object_count; ++i) 
            paint(objects[i], 3),
            objects[i].xpos1 -= 20, 
            objects[i].xpos2 -= 20, 
            paint(objects[i], 12);
    }
    if (ok) _GAME_OVER();
    return 0;
}