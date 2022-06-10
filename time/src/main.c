#include "stdio.h"
#include "time.h"

typedef unsigned long int uint64_t;


/*
 * 获取时间 返回ms
 * Linux specific get abs (wall clock) time in milliseconds function
 *
 * Defined in hlos_specific.h
 */
uint64_t _TIMER_getAbsNow(void)
{
    struct timespec tv;
    uint64_t r,s,ns,ms_from_ns,ms_from_s;

    clock_gettime(CLOCK_REALTIME, &tv);
    s = tv.tv_sec;
    ns = tv.tv_nsec;



    /* 纳秒转毫秒 */
    ms_from_ns = ns / (1000 * 1000);

    /* 秒转毫秒并累加*/
    ms_from_s =  s * 1000;

    r = ms_from_ns+ms_from_s;

#if 0
	printf("tv.tv_sec :%ld\n",s); // 表示距离 1970-01-01 08:00:00 的总s数  1642388620
	printf("tv.tv_nsec :%ld\n",ns); // 887352293 //纳秒数
    printf("ms_from_ns:%ld\n",ms_from_ns);
    printf("ms_from_s:%ld\n",ms_from_s);
    printf("r:%ld\n",r);
#endif

    return (r);
}

// typedef long time_t;
// #ifndef _TIMESPEC
// #define _TIMESPEC
// struct timespec {
// time_t tv_sec; // seconds
// long tv_nsec; // and nanoseconds
// };
// #endif
// struct timespec有两个成员，一个是秒，一个是纳秒, 所以最高精确度是纳秒。
// 一般由函数int clock_gettime(clockid_t, struct timespec *)获取特定时钟的时间，常用如下4种时钟：
// CLOCK_REALTIME 统当前时间，从1970年1.1日算起
// CLOCK_MONOTONIC 系统的启动时间，不能被设置
// CLOCK_PROCESS_CPUTIME_ID 本进程运行时间
// CLOCK_THREAD_CPUTIME_ID 本线程运行时间
// struct tm *localtime(const time_t *clock);  //线程不安全
// struct tm* localtime_r( const time_t* timer, struct tm* result );//线程安全
// size_t strftime (char* ptr, size_t maxsize, const char* format,const struct tm* timeptr );

/*
 * 在当前时间上增加设定毫秒并转换出timespec 
 */
void make_timespec(struct timespec *tv, long int timeout_mSecs)
{
    long int r;

    clock_gettime(CLOCK_REALTIME, tv);
    tv->tv_sec += (timeout_mSecs / 1000);
    /* get just the mSecs portion */
    r = timeout_mSecs % 1000;
    /* convert to nSecs */
    r = r * 1000 * 1000;
    tv->tv_nsec += r;
    while(tv->tv_nsec > (1000 * 1000 * 1000))
    {
            tv->tv_sec += 1;
            tv->tv_nsec -= (1000 * 1000 * 1000);
    }
}


// struct timeval {
// time_t tv_sec; // seconds
// long tv_usec; // microseconds
// };
// struct timezone{
// int tz_minuteswest; //miniutes west of Greenwich
// int tz_dsttime; //type of DST correction
// };

// struct timeval有两个成员，一个是秒，一个是微秒, 所以最高精确度是微秒。
// 一般由函数int gettimeofday(struct timeval *tv, struct timezone *tz)获取系统的时间


int main(int argc, char *argv[])
{
    struct timespec tv;

	uint64_t ms = _TIMER_getAbsNow();
	printf("获取距离 1970-01-01 08:00:00 的总毫秒数:%ld\n",ms);
    // 1642389471033 => 2022-01-17 11:17:51


    printf("获取在当前时间基础上累加设定 时间(10000 毫秒)的timespec\n");
    make_timespec(&tv,10000);
    printf("tv.tv_sec :%ld\n",tv.tv_sec); // 表示距离 1970-01-01 08:00:00 的总s数  1642388620
	printf("tv.tv_nsec :%ld\n",tv.tv_nsec); // 887352293 //纳秒数
}

