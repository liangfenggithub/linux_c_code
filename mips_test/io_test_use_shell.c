#include "stdio.h"
#include"stdlib.h"
#include "time.h"
#include <unistd.h> 

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


// echo 0 > /sys/devices/platform/leds/leds/led3/brightness;
// echo 1 > /sys/devices/platform/leds/leds/led3/brightness;

    while(1)
    {
        clock_gettime(CLOCK_REALTIME, &tv);
        // printf(" clock_gettime tv.tv_sec ld is :%ld tv.tv_sec d is %d\n",tv.tv_sec,tv.tv_sec);
        // printf(" clock_gettime tv.tv_nsec ld is :%ld tv.tv_nsec d is %d\n",tv.tv_nsec,tv.tv_nsec);
        system("echo 0 > /sys/devices/platform/leds/leds/led3/brightness");
        // usleep(1000);
        system("echo 1 > /sys/devices/platform/leds/leds/led3/brightness");
        // usleep(1000);
    }

    return;
}

/*

编译指令
/home/luke/openwrt/openwrt_19.07/staging_dir/toolchain-mipsel_24kc_gcc-7.5.0_musl/bin/mipsel-openwrt-linux-gcc io_test.c -L /home/luke/openwrt/openwrt_19.07/staging_dir/target-mipsel_24kc_musl/usr/include -o b
scp ./b  root@192.168.31.130:/tmp/b
 */