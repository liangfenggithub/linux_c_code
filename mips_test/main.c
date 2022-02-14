#include "stdio.h"
#include "time.h"

// #include <bits/types.h>
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long int uint64_t;// 注意在mips中 64位为 longlong int 而且prinf输出需要使用%lld

unsigned long long int r = 0;

void main(void)
{
    // uint64_t r = 0;
    struct timespec tv;
    printf("333 sizeof(r) is %d\n",sizeof(r));
    r = 1;
    printf("r d is  %lld\n",r); 
    printf("r ld is  %lld\n",r);

    clock_gettime(CLOCK_REALTIME, &tv);
    printf(" clock_gettime tv.tv_sec ld is :%ld tv.tv_sec d is %d\n",tv.tv_sec,tv.tv_sec);
    printf(" clock_gettime tv.tv_nsec ld is :%ld tv.tv_nsec d is %d\n",tv.tv_nsec,tv.tv_nsec);

    /* convert nano seconds to milliseconds */
    r = tv.tv_nsec / (1000 * 1000);
    /* convert secs to mSecs and add */
    printf("r = tv.tv_nsec / (1000 * 1000) is %lld\n",r);
    r = r + ((unsigned long long int)tv.tv_sec * 1000);
    printf("return r ld is :%lld sec d is %d\n",r,r);

    return;
}

/*

 mipsel-openwrt-linux-gcc main.c -L /home/luke/openwrt/openwrt_19.07/staging_dir/target-mipsel_24kc_musl/usr/include -o b
scp ./b  root@192.168.3.244:/tmp/b


 */