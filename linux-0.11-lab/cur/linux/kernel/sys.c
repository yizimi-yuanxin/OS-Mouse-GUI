/*
 *  linux/kernel/sys.c
 *
 *  (C) 1991  Linus Torvalds
 */

#include <errno.h>
#include <unistd.h>
#include <linux/sched.h>
#include <linux/tty.h>
#include <linux/kernel.h>
#include <asm/segment.h>
#include <sys/times.h>
#include <sys/utsname.h>
#include <asm/io.h>

int sys_ftime()
{
	return -ENOSYS;
}

int sys_break()
{
	return -ENOSYS;
}

int sys_ptrace()
{
	return -ENOSYS;
}

int sys_stty()
{
	return -ENOSYS;
}

int sys_gtty()
{
	return -ENOSYS;
}

int sys_rename()
{
	return -ENOSYS;
}

int sys_prof()
{
	return -ENOSYS;
}

int sys_setregid(int rgid, int egid)
{
	if (rgid>0) {
		if ((current->gid == rgid) || 
		    suser())
			current->gid = rgid;
		else
			return(-EPERM);
	}
	if (egid>0) {
		if ((current->gid == egid) ||
		    (current->egid == egid) ||
		    suser()) {
			current->egid = egid;
			current->sgid = egid;
		} else
			return(-EPERM);
	}
	return 0;
}

int sys_setgid(int gid)
{
/*	return(sys_setregid(gid, gid)); */
	if (suser())
		current->gid = current->egid = current->sgid = gid;
	else if ((gid == current->gid) || (gid == current->sgid))
		current->egid = gid;
	else
		return -EPERM;
	return 0;
}

int sys_acct()
{
	return -ENOSYS;
}

int sys_phys()
{
	return -ENOSYS;
}

int sys_lock()
{
	return -ENOSYS;
}

int sys_mpx()
{
	return -ENOSYS;
}

int sys_ulimit()
{
	return -ENOSYS;
}

int sys_time(long * tloc)
{
	int i;

	i = CURRENT_TIME;
	if (tloc) {
		verify_area(tloc,4);
		put_fs_long(i,(unsigned long *)tloc);
	}
	return i;
}

/*
 * Unprivileged users may change the real user id to the effective uid
 * or vice versa.
 */
int sys_setreuid(int ruid, int euid)
{
	int old_ruid = current->uid;
	
	if (ruid>0) {
		if ((current->euid==ruid) ||
                    (old_ruid == ruid) ||
		    suser())
			current->uid = ruid;
		else
			return(-EPERM);
	}
	if (euid>0) {
		if ((old_ruid == euid) ||
                    (current->euid == euid) ||
		    suser()) {
			current->euid = euid;
			current->suid = euid;
		} else {
			current->uid = old_ruid;
			return(-EPERM);
		}
	}
	return 0;
}

int sys_setuid(int uid)
{
/*	return(sys_setreuid(uid, uid)); */
	if (suser())
		current->uid = current->euid = current->suid = uid;
	else if ((uid == current->uid) || (uid == current->suid))
		current->euid = uid;
	else
		return -EPERM;
	return(0);
}

int sys_stime(long * tptr)
{
	if (!suser())
		return -EPERM;
	startup_time = get_fs_long((unsigned long *)tptr) - jiffies/HZ;
	return 0;
}

int sys_times(struct tms * tbuf)
{
	if (tbuf) {
		verify_area(tbuf,sizeof *tbuf);
		put_fs_long(current->utime,(unsigned long *)&tbuf->tms_utime);
		put_fs_long(current->stime,(unsigned long *)&tbuf->tms_stime);
		put_fs_long(current->cutime,(unsigned long *)&tbuf->tms_cutime);
		put_fs_long(current->cstime,(unsigned long *)&tbuf->tms_cstime);
	}
	return jiffies;
}

int sys_brk(unsigned long end_data_seg)
{
	if (end_data_seg >= current->end_code &&
	    end_data_seg < current->start_stack - 16384)
		current->brk = end_data_seg;
	return current->brk;
}

/*
 * This needs some heave checking ...
 * I just haven't get the stomach for it. I also don't fully
 * understand sessions/pgrp etc. Let somebody who does explain it.
 */
int sys_setpgid(int pid, int pgid)
{
	int i;

	if (!pid)
		pid = current->pid;
	if (!pgid)
		pgid = current->pid;
	for (i=0 ; i<NR_TASKS ; i++)
		if (task[i] && task[i]->pid==pid) {
			if (task[i]->leader)
				return -EPERM;
			if (task[i]->session != current->session)
				return -EPERM;
			task[i]->pgrp = pgid;
			return 0;
		}
	return -ESRCH;
}

int sys_getpgrp(void)
{
	return current->pgrp;
}

int sys_setsid(void)
{
	if (current->leader && !suser())
		return -EPERM;
	current->leader = 1;
	current->session = current->pgrp = current->pid;
	current->tty = -1;
	return current->pgrp;
}

int sys_getgroups()
{
	return -ENOSYS;
}

int sys_setgroups()
{
	return -ENOSYS;
}

int sys_uname(struct utsname * name)
{
	static struct utsname thisname = {
		"linux .0","nodename","release ","version ","machine "
	};
	int i;

	if (!name) return -ERROR;
	verify_area(name,sizeof *name);
	for(i=0;i<sizeof *name;i++)
		put_fs_byte(((char *) &thisname)[i],i+(char *) name);
	return 0;
}

int sys_sethostname()
{
	return -ENOSYS;
}

int sys_getrlimit()
{
	return -ENOSYS;
}

int sys_setrlimit()
{
	return -ENOSYS;
}

int sys_getrusage()
{
	return -ENOSYS;
}

int sys_gettimeofday()
{
	return -ENOSYS;
}

int sys_settimeofday()
{
	return -ENOSYS;
}


int sys_umask(int mask)
{
	int old = current->umask;

	current->umask = mask & 0777;
	return (old);
}

int flag = 0;
#define vga_graph_memstart 	0xA0000
#define vga_graph_memsize  	64000
#define cursor_size  		6
#define vga_width			320
#define vga_height			200

int sys_init_graphics(void) {
	if (!flag) {
		outb(0x05, 0x3CE); 			// 图形控制器端口 0x3CE 地址端口 0x05 位置
		outb(0x40, 0x3CF);			// 设定 shift256 为 1  数据端口
									// 即设定 256 色，且取出方式为移动拼装
		outb(0x06, 0x3CE);			// 转到 Memory Map Select 位  
		outb(0x05, 0x3CF);			// 设定其为 01 的显存区域 且禁止字符模式

		outb(0x04, 0x3C4);			// Sequencer Memory Mode Register 寄存器第四位
		outb(0x08, 0x3C5);			// 即为 Chain4 位 设定 4 个显存片连在一起

		outb(0x01, 0x3D4);			// 设置 CRT 控制器
		outb(0x4F, 0x3D5);			// 设置 End Horizontal Display 为 79

		outb(0x03, 0x3D4);			// 设置 End Horizontal Blanking
		outb(0x82, 0x3D5);			// 设置其中 Display Enable Skew 为 0x82

		outb(0x07, 0x3D4);			// 设置 Vertical Display End 8-9 位
		outb(0x1F, 0x3D5);			// 设置为 0x1F, 其实际值为 399

		outb(0x12, 0x3D4); 			// 设置 Vertical Display End 0-7 位
		outb(0x8F, 0x3D5);			// 设置为 0x8F, 其实际值为 399

		outb(0x17, 0x3D4);
		outb(0xA3, 0x3D5);			// 不是很懂 SLDIV 是个啥啊QAQ

		outb(0x14, 0x3D4);			// 设置 Memory Address Size
		outb(0x40, 0x3D5); 			// 设置 DW = 1 (第六位)

		outb(0x13, 0x3D4);
		outb(0x28, 0x3D5);			// 设置 Offset 为 40

		outb(0x0C, 0x3D4);
		outb(0x00, 0x3D5);			// 高 8 位

		outb(0x0D, 0x3D4);			
		outb(0x00, 0x3D5); 			// 低 8 位

		flag = 1;
	}
	int i, j, x, y;
	char *p;

	p = vga_graph_memstart;
	for (i = 0; i < vga_graph_memsize; ++i)  *(p++) = 3;

	// x = 20, y = 10;
	// for (i = x - cursor_size; i <= x + cursor_size; ++i) {
	// 	for (j = y - cursor_size; j <= y + cursor_size; ++j) {
	// 		p = (char*)vga_graph_memstart + j * vga_width + i;
	// 		*p = 12; 
	// 	}
	// }
}
extern int mouse_x_pos;
extern int mouse_y_pos;

int sys_get_mouse_posx() {
	return mouse_x_pos;
}

int sys_get_mouse_posy() {
	return mouse_y_pos;
}

int sys_get_message(int *msg) {
	if (msg_queue_tail == msg_queue_head) {
		put_fs_long(-1, msg);
		return -1;
	}
	int message_ret = msg_queue[msg_queue_head].index;
	msg_queue[msg_queue_head].index = 0;
	msg_queue_head = (msg_queue_head + 1) % MAX_MSG;
	put_fs_long(message_ret, msg);
	return 0;
}

void post_message(int from) {
	// printk("post message: %d\n", from);
	if (msg_queue_head - 1 != msg_queue_tail) {
		// printk("post message: %d\n", from);
		message msg;
		msg.index = from;
		msg.pid = current->pid;
		msg_queue[msg_queue_tail] = msg;
		msg_queue_tail = (msg_queue_tail + 1) % MAX_MSG;
	}
	return;
}

int sys_timer_create(int millsoconds, int type) {
	int jiffies = millsoconds / 10;
	user_timer *timer = (user_timer*)malloc(sizeof(user_timer));
	timer->init_jiffies = jiffies;
	timer->jiffies = jiffies;
	timer->pid = current->pid;
	timer->type = type;
	timer->next = timer_head;
	timer_head = timer;
	return 0;
}

// int sys_repaint(paint_pos *pos, char x) {
// 	int i, j;
// 	paint_pos tp;
// 	tp.xpos1 = get_fs_long(pos), 	 tp.xpos2 = get_fs_long(pos + 1);
// 	tp.ypos1 = get_fs_long(pos + 2), tp.ypos2 = get_fs_long(pos + 3);
// 	// printk("repaint: %d %d %d %d\n", tp.xpos1, tp.xpos2, tp.ypos1, tp.ypos2);
// 	if (tp.xpos1 < 0 || tp.xpos1 >= vga_width)	return -1;
// 	if (tp.xpos2 < 0 || tp.xpos2 >= vga_width)	return -1;
// 	if (tp.ypos1 < 0 || tp.ypos1 >= vga_height) return -1;
// 	if (tp.ypos2 < 0 || tp.ypos2 >= vga_height) return -1;
// 	if (tp.xpos1 > tp.xpos2)					return -1;
// 	if (tp.ypos1 > tp.ypos2)					return -1;
	
// 	char *p = vga_graph_memstart;
// 	for (i = tp.xpos1; i <= tp.xpos2; ++i) {
// 		for (j = tp.ypos1; j <= tp.ypos2; ++j) {
// 			p = (char *)vga_graph_memstart + j * vga_width + i,
// 			*p = 12;
// 		}
// 	}
	
// 	return 0;
// }


int sys_repaint(int xpos, int ypos, char x) {
	int i, j, xpos1, xpos2, ypos1, ypos2;
	xpos1 = xpos >> 9;
	xpos2 = xpos % 512;
	ypos1 = ypos >> 9;
	ypos2 = ypos % 512;
	// printk("repaint: %d %d %d %d\n", xpos1, xpos2, ypos1, ypos2);
	if (xpos1 < 0 || xpos1 >= vga_width)	return -1;
	if (xpos2 < 0 || xpos2 >= vga_width)	return -1;
	if (ypos1 < 0 || ypos1 >= vga_height) 	return -1;
	if (ypos2 < 0 || ypos2 >= vga_height) 	return -1;
	if (xpos1 > xpos2)						return -1;
	if (ypos1 > ypos2)						return -1;
	
	char *p = vga_graph_memstart;
	for (i = xpos1; i <= xpos2; ++i) {
		for (j = ypos1; j <= ypos2; ++j) {
			p = (char *)vga_graph_memstart + j * vga_width + i,
			*p = x;
		}
	}
	
	return 0;
}