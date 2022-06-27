target remote localhost:1234
handle SIGSEGV nostop noprint ignore
b main
display current->pid
c
