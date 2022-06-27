/*
 *  by yizimi - 2022/06/27
 *  mouse_interrupt
 */

#include <linux/config.h>

.text
.globl mouse_interrupt


mouse_interrupt:
	pushl %eax
	pushl %ebx
	pushl %ecx
	pushl %edx
	push %ds
	movl $0x10, %eax	
	mov  %ax, %ds		/* 为后面的 call 做准备 */
	inb  $0x60, %al		/* 鼠标数据寄存器端口为 0x60 */
	call readmouse		/* 调用 C 函数 */
	addl $4,%esp
	pop %es
	pop %ds
	popl %edx
	popl %ecx
	popl %ebx
	popl %eax
	iret