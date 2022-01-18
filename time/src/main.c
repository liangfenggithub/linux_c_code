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

