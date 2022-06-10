#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <sched.h>
#include <assert.h>

void show_thread_info()
{
	int err;
	int policy;
	struct sched_param schedule_parameters;

	printf("--- Show policy and priority of current thread:\n");
	err = pthread_getschedparam(pthread_self(), &policy, &schedule_parameters);
	assert(!err);
	switch (policy)
	{
	case SCHED_FIFO:
		printf("--- policy = SCHED_FIFO\n");
		break;
	case SCHED_RR:
		printf("--- policy = SCHED_RR\n");
		break;
	case SCHED_OTHER:
		printf("--- policy = SCHED_OTHER\n");
		break;
	default:
		printf("--- policy = UNKNOWN\n");
		break;
	}
	printf("--- Current thread's sched_priority = %d\n", schedule_parameters.sched_priority);
	printf("--- Current thread's max_priority = %d\n", sched_get_priority_max(policy));
	printf("--- Current thread's min_priority = %d\n", sched_get_priority_min(policy));
}

void *start_routine(void *arg)
{
	int i, j;
	show_thread_info();
	while (1)
	{
		fprintf(stderr, "%c ", *(char *)arg);
		for (i = 0; i < 100000; i++)
			for (j = 0; j < 1000; j++)
				;
	}

	pthread_exit(NULL);
}
int main(void)
{

	pthread_t tid1, tid2, tid3;
	pthread_attr_t attr1, attr2;
	struct sched_param param1, param2;

	/* 线程属性变量的初始化 */
	pthread_attr_init(&attr1);
	pthread_attr_init(&attr2);

	/* 设置线程是否继承创建者的调度策略 PTHREAD_EXPLICIT_SCHED：不继承才能设置线程的调度策略*/
	errno = pthread_attr_setinheritsched(&attr1, PTHREAD_EXPLICIT_SCHED);
	if (errno != 0)
	{
		perror("setinherit failed\n");
		return -1;
	}

	/* 设置线程是否继承创建者的调度策略 PTHREAD_EXPLICIT_SCHED：不继承才能设置线程的调度策略*/
	errno = pthread_attr_setinheritsched(&attr2, PTHREAD_EXPLICIT_SCHED);
	if (errno != 0)
	{
		perror("setinherit failed\n");
		return -1;
	}

	/* 设置线程的调度策略：
	SCHED_FIFO：抢占性调度; SCHED_RR：轮寻式调度；SCHED_OTHER：非实时线程调度策略
	注意：当线程调度策略 是 SCHED_FIFO或者SCHED_RR时必须使用root用户执行程序

	*/
	errno = pthread_attr_setschedpolicy(&attr1, SCHED_RR);
	if (errno != 0)
	{
		perror("setpolicy failed\n");
		return -1;
	}

	errno = pthread_attr_setschedpolicy(&attr2, SCHED_RR);
	if (errno != 0)
	{
		perror("setpolicy failed\n");
		return -1;
	}

	//设置优先级的级别
	param1.sched_priority = 1;
	param2.sched_priority = 1;

	//查看抢占性调度策略的最小跟最大静态优先级的值是多少
	printf("min=%d, max=%d\n", sched_get_priority_min(SCHED_FIFO), sched_get_priority_max(SCHED_FIFO));

	/* 设置线程静态优先级 */
	errno = pthread_attr_setschedparam(&attr1, &param1);
	if (errno != 0)
	{
		perror("setparam failed\n");
		return -1;
	}

	errno = pthread_attr_setschedparam(&attr2, &param2);
	if (errno != 0)
	{
		perror("setparam failed\n");
		return -1;
	}

	/* 创建三个测试线程 */
	/* 线程1，优先级1 */
	errno = pthread_create(&tid1, &attr1, start_routine, (void *)"1");
	if (errno != 0)
	{
		perror("create thread 1 failed\n");
		return -1;
	}
	/* 线程2，优先级2 */
	errno = pthread_create(&tid2, &attr2, start_routine, (void *)"2");
	if (errno != 0)
	{
		perror("create thread 2 failed\n");
		return -1;
	}
	/* 线程3，非实时线程，静态优先级0 */
	errno = pthread_create(&tid3, NULL, start_routine, (void *)"3");
	if (errno != 0)
	{
		perror("create thread 3 failed\n");
		return -1;
	}

	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);
	pthread_join(tid3, NULL);

	pthread_attr_destroy(&attr1);
	pthread_attr_destroy(&attr2);

	return 0;
}

/* 编译指令
/home/luke/openwrt/openwrt_19.07/staging_dir/toolchain-mipsel_24kc_gcc-7.5.0_musl/bin/mipsel-openwrt-linux-gcc ./main.c -L /home/luke/openwrt/openwrt_19.07/staging_dir/target-mipsel_24kc_musl/usr/include -lpthread -o a
scp ./a  root@192.168.0.130:/tmp/a

 */