#include "stdio.h"
#include "time.h"
#include <pthread.h>
#include <assert.h>
#include <string.h>
// #include <errno.h>

typedef unsigned long int uint64_t;


static void set_thread_policy( pthread_attr_t *attr,  int policy )
{
        int rs = pthread_attr_setschedpolicy( attr, policy );
        assert( rs == 0 );
}

void show_thread_info()
{
    int err;
    int policy;
    struct sched_param schedule_parameters;
 
    printf("--- Show policy and priority of current thread:\n");
    err = pthread_getschedparam(pthread_self(), &policy, &schedule_parameters );
    assert( !err );
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
    printf("--- Current thread's max_priority = %d\n", sched_get_priority_max( policy ));
    printf("--- Current thread's min_priority = %d\n", sched_get_priority_min( policy ));

}
int create_comm_thread(pthread_t *pth, void*(*func)(void *), void *arg,
		     int policy, int prio)
{
	int ret;
	struct sched_param schedp;
	pthread_attr_t attr;

	// printf("arg is %d\n",*(int *)arg);

	pthread_attr_init(&attr);
	memset(&schedp, 0, sizeof(schedp));

	ret = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
	if (ret) {
		printf("pthread_attr_setinheritsched:%d\n", ret);
		printf("pthread_attr_setinheritsched:%s\n", strerror(ret));

		return -1;
	}

	ret = pthread_attr_setschedpolicy(&attr, policy);
	if (ret) {
		printf("pthread_attr_setschedpolicy:%d\n", ret);
		printf("pthread_attr_setschedpolicy:%s\n", strerror(ret));

		return -1;
	}

	schedp.sched_priority = prio;
	ret = pthread_attr_setschedparam(&attr, &schedp);
	if (ret) {
		printf("pthread_attr_setschedparam:%d\n", ret);
		printf("pthread_attr_setschedparam:%s\n", strerror(ret));

		return -1;
	}

	ret = pthread_create(pth, &attr, func, arg);
	if (ret) {
		printf("pthread_create:%d\n", ret);
		printf("pthread_create:%s\n", strerror(ret));

		return -1;
	}
	return 0;
}
void *thread1(void * cookie)
{
    printf("线程1 执行开始\n");
    show_thread_info();
    sleep(1);

    while(1)
    {
        printf("1");
        // printf("i am thread 1\n");
    }
    printf("线程1 执行完毕\n");

}

void *thread2(void * cookie)
{
    printf("线程2 执行开始\n");
    show_thread_info();
    sleep(1);
    while(1)
    {
        printf("2");
        // printf("i am thread 2\n");
    }
    printf("线程2 执行完毕\n");

}

void *thread3(void * cookie)
{
    printf("线程3 执行开始\n");
    show_thread_info();
    sleep(1);

    while(1)
    {
        printf("3");
        // printf("i am thread 3\n");
    }
    printf("线程3 执行完毕\n");

}

int main(int argc, char *argv[])
{
    pthread_t pids[10];
	pthread_attr_t attr;
	struct sched_param sched;
    void* thread_result;
	int rs;
#if 0
	rs = pthread_attr_init( &attr );
	assert( rs == 0 );

	printf("1\n");
	show_thread_info();

	printf("2\n");
	set_thread_policy( &attr, SCHED_FIFO);
	show_thread_info();

	printf("3\n");
	set_thread_policy( &attr, SCHED_RR);
	show_thread_info();
	return;
#else
	printf("hello pthread\n");

    printf("默认调度策略和最大最小优先级\n");
    show_thread_info();

//注意:必须用root用户执行程序才能看到效果,否则程序不能运行
// SCHED_OTHER 不能设置大于0的优先级,最大最小优先级都为0
// SCHED_FIFO  优先级1-99 
// SCHED_RR 优先级1-99
//SCHED_FIFO：抢占性调度; SCHED_RR：轮寻式调度；SCHED_OTHER：非实时线程调度策略
// SCHED_FIFO和 SCHED_RR的区别是:当多个线程设置为同一个优先级,前者会使一个线程一直占用CPU资源,而后者会均匀分配多个线程的资源占用
//也就是说SCHED_RR在调度上除了优先级控制,还有时间片的控制,控制统一优先级的线程运行一段时候后释放cpu资源
    create_comm_thread(&(pids[0]),thread1,NULL,SCHED_RR,1);
    create_comm_thread(&(pids[1]),thread2,NULL,SCHED_RR,2);
    create_comm_thread(&(pids[2]),thread3,NULL,SCHED_RR,3);



    //阻塞主线程，直至 mythread1 线程执行结束，用 thread_result 指向接收到的返回值，阻塞状态才消除。
    rs = pthread_join(pids[0], NULL);
    rs = pthread_join(pids[1], NULL);
    rs = pthread_join(pids[2], NULL);

    printf("主线程执行完毕\n");
#endif
}

