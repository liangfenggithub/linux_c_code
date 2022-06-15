#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/rtc.h>
#include <linux/i2c.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
/*
RTC_SET_TIME
RTC_RD_TIME
*/
#define PCF8563_ADDR 0X51
struct rtc_time rtc_time_ins;
struct rtc_time r_time;
struct tm tm_time;
int second,minute = 0;
int fd;


/************************第一部分***********************/
 uint8_t I2C0_Init(void)
{
    fd  = open("/dev/i2c-0", O_RDWR);               // 以读写权限打开IIC 1设备文件

    if (fd < 0)
    {
        perror("Can't open /dev/i2c-0 \n");
        exit(1);
    }

    printf("open /dev/i2c-0 success !\n");

    //ioctl(fd,I2C_TENBIT,0);                  //7位模式 
    if (ioctl(fd, I2C_SLAVE_FORCE, PCF8563_ADDR) < 0)         //设置IIC设备地址
    {
        printf("fail to set i2c device slave address!\n");
        close(fd);
        return - 1;
    }

    printf("set slave address to 0x%x success!\n", PCF8563_ADDR);
    return (0);
}

 uint8_t I2C0_DeInit(void)
{
    close(fd);
}

/************************第二部分***********************/
//MPU6050 wirte byte
 uint8_t i2c_write(int fd, uint8_t reg, uint8_t *val,uint8_t cnt)
{
    uint8_t data[255];
    int num;

    data[0] = reg;
    for(num = 0; num< cnt ;num++)
    {
        data[1+num] = val[num];
    }

    if (write(fd, data, (cnt+1)) == (cnt+1))
    {
        return 0;

    }

    return - 1;
}

 uint8_t i2c_write_byte(int fd, uint8_t reg, uint8_t val)
{
    uint8_t data[2];

    data[0] = reg;
    data[1] = val;

    if (write(fd, data, 2) == 2)
    {
        return 0;

    }

    return - 1;
}

/************************第三部分***********************/
//MPU6050 read byte
 uint8_t i2c_read(int fd, uint8_t reg, uint8_t * val,uint8_t cnt)
{
    if (write(fd, &reg, 1) == 1)
    {
        if (read(fd, val, cnt) == cnt)
        {
            return 0;
        }

    }
    return - 1;
}

/******************BCD转十进制***************************/
uint8_t bcd_dec(uint8_t bat)
{
	uint8_t temp1,temp2,tol;
	temp1=bat&0x0f;
	temp2=(bat&0xf0)>>4;
	tol=temp2*10+temp1;
	return tol; 
}
/******************十进制转BCD***************************/
uint8_t dec_bcd(uint8_t bat)
{
	return ((bat%10) & 0x0F) | (((bat/10) << 4) & 0xF0);; 
}

void P8563_alarm(uint8_t day,uint8_t hour,uint8_t min)
{
		day = dec_bcd(day);
		hour = dec_bcd(hour);
		min = dec_bcd(min);	
	    i2c_write_byte(fd,0xc,0x1);   //星期报警
	    
	    i2c_write_byte(fd,0xb,day); /*日期报警*/	
        i2c_write_byte(fd,0xa,hour); /*小时报警*/	
	    i2c_write_byte(fd,0x9,min); /*分钟报警*/
	
     	i2c_write_byte(fd,0x01,0x02); /*报警有效*/ //使能报警器和定时器发出中断请求
}

void P8563_min_alarm(uint8_t min)
{
		min = dec_bcd(min);	
	    
	    i2c_write_byte(fd,0x9,min); /*分钟报警*/
        i2c_write_byte(fd,0xa,0x80); /*忽略小时报警*/	
	    i2c_write_byte(fd,0xb,0x80); /*忽略日期报警*/	
	    i2c_write_byte(fd,0xc,0x80); //忽略星期报警
	
     	i2c_write_byte(fd,0x01,0x02); //使能报警器和定时器发出中断请求
}
/*

编译指令
/home/luke/openwrt/openwrt_19.07/staging_dir/toolchain-mipsel_24kc_gcc-7.5.0_musl/bin/mipsel-openwrt-linux-gcc i2c_test.c -L /home/luke/openwrt/openwrt_19.07/staging_dir/target-mipsel_24kc_musl/usr/include -o i2c_test
scp ./i2c_test  root@192.168.0.131:/tmp/i2c_test
 */
int main(int argc, char **argv)
{

    if(0 != I2C0_Init())
    {
        printf("error\n");
        return;
    }

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
 
        minute = (tm_time.tm_min+1)%60;

        printf("目标分数是%d\n",minute);

        P8563_min_alarm(minute);
        sleep(61);
        

    }

    return 0;
}