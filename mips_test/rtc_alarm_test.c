#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/rtc.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
/*
RTC_SET_TIME
RTC_RD_TIME
*/

struct rtc_time rtc_time_ins;
struct rtc_time r_time;
struct tm tm_time;
int second = 0;
/*

编译指令
/home/luke/openwrt/openwrt_19.07/staging_dir/toolchain-mipsel_24kc_gcc-7.5.0_musl/bin/mipsel-openwrt-linux-gcc rtc_alarm_test.c -L /home/luke/openwrt/openwrt_19.07/staging_dir/target-mipsel_24kc_musl/usr/include -o rtc_alarm_test
scp ./rtc_alarm_test  root@192.168.0.131:/tmp/rtc_alarm_test
 */
int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("./app /dev/rtcX \n");
        return 0;
    }
    // 1.打开设备文件
    int fd = open(argv[1], 2);
    if (fd < 2)
    {
        printf("%s 设备文件打开失败.\n", argv[1]);
        return 0;
    }

#if 0
    // 3.获取RTC驱动的时间
    ioctl(fd, RTC_RD_TIME, &rtc_time_ins);
    printf("当前RTC的时间: %d-%d-%d %d:%d:%d\n",
            rtc_time_ins.tm_year + 1900,
            rtc_time_ins.tm_mon + 1,
            rtc_time_ins.tm_mday,
            rtc_time_ins.tm_hour,
            rtc_time_ins.tm_min,
            rtc_time_ins.tm_sec);

    //4. 获取当前系统时间
    time_t sec_time=time(NULL);
    printf("当前系统的总秒数:%d ",sec_time);
    localtime_r(&sec_time,&tm_time);
    printf("及对应本地时间: %d-%d-%d %d:%d:%d\n",tm_time.tm_year+1900,
                                tm_time.tm_mon+1,
                                tm_time.tm_mday,
                                tm_time.tm_hour,
                                tm_time.tm_min,
                                tm_time.tm_sec);

    //5.写入系统时间到rtc中
    rtc_time_ins.tm_year = tm_time.tm_year ;
    rtc_time_ins.tm_mon  = tm_time.tm_mon  ;
    rtc_time_ins.tm_mday = tm_time.tm_mday ;
    rtc_time_ins.tm_hour = tm_time.tm_hour ;
    rtc_time_ins.tm_min  = tm_time.tm_min  ; 
    rtc_time_ins.tm_sec  = tm_time.tm_sec  ;
    if(0!=ioctl(fd,RTC_SET_TIME,&rtc_time_ins))
    {
        perror("RTC_SET_TIME\n");
    }

    ioctl(fd, RTC_RD_TIME, &rtc_time_ins);
    printf("写入后再次读取RTC的时间: %d-%d-%d %d:%d:%d\n",
            rtc_time_ins.tm_year + 1900,
            rtc_time_ins.tm_mon + 1,
            rtc_time_ins.tm_mday,
            rtc_time_ins.tm_hour,
            rtc_time_ins.tm_min,
            rtc_time_ins.tm_sec);

#endif


    while (1)
    {





        //4. 获取当前系统时间
        time_t sec_time=time(NULL);
        printf("当前系统的总秒数:%d ",sec_time);
        localtime_r(&sec_time,&tm_time);
        printf("及对应本地时间: %d-%d-%d %d:%d:%d\n",tm_time.tm_year+1900,
                                    tm_time.tm_mon+1,
                                    tm_time.tm_mday,
                                    tm_time.tm_hour,
                                    tm_time.tm_min,
                                    tm_time.tm_sec);
        //分钟加1设置设置alarm时间
        second = sec_time%60;
        sec_time = sec_time-second+60; 
        printf("余数是: %d 目标秒数是%d\n",second,sec_time);

        //转成本地时间
        localtime_r(&sec_time,&tm_time);
        //设置RTC alarm 到期时间                     
        rtc_time_ins.tm_year = tm_time.tm_year ;
        rtc_time_ins.tm_mon  = tm_time.tm_mon  ;
        rtc_time_ins.tm_mday = tm_time.tm_mday ;
        rtc_time_ins.tm_hour = tm_time.tm_hour ;
        rtc_time_ins.tm_min  = tm_time.tm_min  ; 
        rtc_time_ins.tm_sec  = tm_time.tm_sec  ;

        printf("设置RTC中断输出的时间: %d-%d-%d %d:%d:%d\n",
               rtc_time_ins.tm_year+1900,
               rtc_time_ins.tm_mon+1,
               rtc_time_ins.tm_mday,
               rtc_time_ins.tm_hour,
               rtc_time_ins.tm_min,
               rtc_time_ins.tm_sec);
        if(ioctl(fd, RTC_ALM_SET, &rtc_time_ins))
        {
            perror("RTC_ALM_SET error");
            goto exit;
        }
        // 4. 再次读出确保写入成功
        if(ioctl(fd, RTC_ALM_READ, &rtc_time_ins))
        {
            printf("RTC_ALM_READ error");
            goto exit;

        }
        printf("再次读出RTC中断输出的时间: %d-%d-%d %d:%d:%d\n",
               rtc_time_ins.tm_year+1900,
               rtc_time_ins.tm_mon+1,
               rtc_time_ins.tm_mday,
               rtc_time_ins.tm_hour,
               rtc_time_ins.tm_min,
               rtc_time_ins.tm_sec);
        
        
        // 2.打开alarm中断
        if(0!= ioctl(fd, RTC_AIE_ON, NULL))
        {
            printf("RTC_AIE_ON error");
            goto exit;
        }
        printf("打开RTC alarm 中断输出\n");
        
        // // 3.获取RTC驱动的时间
        if(ioctl(fd, RTC_RD_TIME, &rtc_time_ins))
        {
            printf("RTC_RD_TIME error");
            goto exit;
        }

        printf("当前RTC的时间: %d-%d-%d %d:%d:%d\n",
                rtc_time_ins.tm_year + 1900,
                rtc_time_ins.tm_mon + 1,
                rtc_time_ins.tm_mday,
                rtc_time_ins.tm_hour,
                rtc_time_ins.tm_min,
                rtc_time_ins.tm_sec);


        sleep(59);
        

    }

exit:
    // 5. 关闭驱动
    close(fd);
    return 0;
}