
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#define __NR_init_graphics	92
#define __NR_timer_create   93
#define __NR_paint_rect		94
#define __NR_get_message	95
#define MSG_MOUSE_CLICK_L 1
#define MSG_TIMEOUT     2
#define MSG_MOUSE_CLICK_R 3
int i,j;
typedef struct {
    int mid;
    int pid;
} message;
typedef struct {
    short x1,y1,x2,y2,color;
} object;
object obj[30];
int init_graphics(void) \
{ \
long __res; \
__asm__ volatile ("int $0x80" \
	: "=a" (__res) \
	: "0" (92)); \
if (__res >= 0) \
	return (int) __res; \
errno = -__res; \
return -1; \
}

int timer_create(long a,int b)
{
long __res;
__asm__ volatile ("int $0x80" : "=a" (__res) : "0" (93),"b" ((long)(a)),"c" ((long)(b)));
if (__res >= 0)
	return (int) __res;
errno = -__res;
return -1;
}

int paint_rect(short *p)
{
long __res;
__asm__ volatile ("int $0x80" : "=a" (__res) : "0" (94),"b" ((long)(p)));
if (__res >= 0)
	return (int) __res;
errno = -__res;
return -1;
}
int get_message(message *msg)
{
long __res;
__asm__ volatile ("int $0x80" : "=a" (__res) : "0" (95),"b" ((long)(msg)));
if (__res >= 0)
	return (int) __res;
errno = -__res;
return -1;
}

void paint(object obj) {
	short *p = (short *)malloc(sizeof(short) * 5);
	p[0] = obj.x1; if(p[0] < 0) p[0] = 0; if(p[0] > 320) p[0] = 320;
	p[1] = obj.y1; if(p[1] < 0) p[1] = 0; if(p[1] > 200) p[0] = 200;
	p[2] = obj.x2; if(p[2] < 0) p[2] = 0; if(p[2] > 320) p[0] = 320;
	p[3] = obj.y2; if(p[3] < 0) p[3] = 0; if(p[3] > 200) p[0] = 200;
	p[4] = obj.color;
	paint_rect(p);
}
const int num=5;
void init(){
	int i;
	obj[0].x1 = 0, obj[0].y1 = 0, obj[0].x2 = 320, obj[0].y2 = 200, obj[0].color = 3;
	paint(obj[0]);
	for(i=1;i<=2*num+2;i++){
		if(i%2){
			obj[i].x1 = 0, obj[i].y1 = 200/num*(i/2-1);
			obj[i].x2 = 159, obj[i].y2 = 200/num*((i/2))-1;
			obj[i].color = 1;
		}
		else{
			obj[i].x1 = 160, obj[i].y1 = 200/num*((i-1)/2-1);
			obj[i].x2 = 320, obj[i].y2 = 200/num*((i/2)-1)-1;
			obj[i].color = 1;
		}
		paint(obj[i]);
	}
}

int main() {
	
	message *msg;
	msg->mid=0;
	
	init_graphics();
	init();
	timer_create(1000,1);
	while(1){
		get_message(msg);
		if(msg->mid==MSG_MOUSE_CLICK_L){
			for(i=1;i<=2*num+2;i+=2){
				if(obj[i].y1>=(num-1)*200/num && obj[i].color==12){
					obj[i].color=1;
					paint(obj[i]);
					break;
				}
			}
		}
		if(msg->mid==MSG_MOUSE_CLICK_R){
			for(i=2;i<=2*num+2;i+=2){
				if(obj[i].y1>=(num-1)*200/num && obj[i].color==12){
					obj[i].color=1;
					paint(obj[i]);
					break;
				}
			}
		}
		if(msg->mid==MSG_TIMEOUT){
			for(i=1;i<=2*num+2;i++){
				obj[i].y1 += (200/num);
				obj[i].y2 += (200/num);
				if(obj[i].y1 >= 200){
					obj[i].y1 = - 200/num;
					obj[i].y2 = - 1;
					obj[i].color = 1;
					if(rand()%2) obj[i].color=12;
				}
				paint(obj[i]);
			}
			timer_create(1000-(j),1);
			if(j<500) j+=10;
		}
	}
	return 0;
}
