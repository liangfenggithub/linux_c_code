#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/rtc.h>

/*
RTC_SET_TIME
RTC_RD_TIME
*/

struct rtc_time time;

/*

编译指令
/home/luke/openwrt/openwrt_19.07/staging_dir/toolchain-mipsel_24kc_gcc-7.5.0_musl/bin/mipsel-openwrt-linux-gcc rtc_test.c -L /home/luke/openwrt/openwrt_19.07/staging_dir/target-mipsel_24kc_musl/usr/include -o rtc_test
scp ./rtc_test  root@192.168.0.131:/tmp/rtc_test
 */
int main(int argc,char **argv)
{
    if(argc!=2)
    {
        printf("./app /dev/rtcX \n");
        return 0;
    }
    //1.打开设备文件
    int fd=open(argv[1],2);
    if(fd<2)
    {
        printf("%s 设备文件打开失败.\n",argv[1]);
        return 0;
    }
    
    //2.获取RTC驱动的时间
    ioctl(fd,RTC_RD_TIME,&time);
    printf("应用层读取的时间: %d-%d-%d %d:%d:%d\n",
            time.tm_year+1900,
            time.tm_mon+1,
            time.tm_mday,
            time.tm_hour,
            time.tm_min,
            time.tm_sec);
    
    // //3.设置RTC驱动的时间
    time.tm_year=2021-1900;
    time.tm_mon=10-1;
    time.tm_mday=1;
    time.tm_hour=11;
    time.tm_min=10;
    time.tm_sec=20;
    ioctl(fd,RTC_SET_TIME,&time);
    
    //4. 关闭驱动
    close(fd);
    return 0;
}