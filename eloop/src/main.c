#include "eloop.h"
#include "stdio.h"

int a = 0; 

void time_callback()
{
	printf("timer : %d\n",a++);
	eloop_register_timeout(1, 0, time_callback, NULL, NULL);
}

int main(int argc, char *argv[])
{
    /* 初始化eloop事件,所有调用eloop相关的函数均需要放在eloop_init之后 */
    eloop_init();

	//定时任务
	eloop_register_timeout(1, 0, time_callback, NULL, NULL);

    /* 事件开始运行 会死在里面 */
    eloop_run(); 

	printf("game over\n");
}