#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <poll.h>

#define FILENAME_SIZE  128
 
#define GPIO_PATH "/sys/class/gpio"
#define GPIO_42_PATH "/sys/class/gpio/gpio42"

//判断指定GPIO是否导出
int gpio_is_exported(int pin_number)
{
    char buf[FILENAME_SIZE];
	int  fp;
	int  length;

    length = snprintf(buf, sizeof(buf), "%s/gpio%d",GPIO_PATH,pin_number);
    printf("gpio_is_exported path is %s\n",buf);
    // https://blog.51cto.com/u_15127553/3333423
    // 函数原型： int access(const char *filename, int mode);
    // 所属头文件：io.h
    // filename：可以填写文件夹路径或者文件路径
    // mode：      
    // 0 （F_OK） 只判断是否存在
    // 2 （R_OK） 判断写入权限
    // 4 （W_OK） 判断读取权限
    // 6 （X_OK） 判断执行权限
    if(0 == access(buf, F_OK))//判断该文件夹是否存在
    {
        printf("can access %s\n",buf);
        return 1;
    }
    else
    {
        return 0;
    }
}
 
//导出io
int gpio_export(int pin_number) 
{
	char buf[FILENAME_SIZE];
	int  fp;
	int  length;
 
	if ( gpio_is_exported(pin_number) ) { return 1; }
 
	//write to export file
	fp = open(GPIO_PATH "/export", O_WRONLY);
	if ( fp == -1) 
        {
		return 0;
	}
 
	length = snprintf(buf, sizeof(buf), "%d", pin_number);
    printf("gpio_export path is %s\n",buf);
	if (write(fp, buf, length * sizeof(char)) == -1) 
        {
		close(fp);
		return 0;
	}
 
	close(fp);
	return gpio_is_exported(pin_number);
}
 
static int gpio_config(const char * gpio_path,const char *attr, const char *val)
{
    char file_path[100];
    int len;
    int fd;
    sprintf(file_path, "%s/%s", gpio_path, attr);
    printf("file path is: %s\n",file_path);
    if (0 > (fd = open(file_path, O_WRONLY)))
    {
        perror("gpio open error");
        return fd;
    }
    len = strlen(val);
    if (len != write(fd, val, len))
    {
        perror("gpio write error");
        close(fd);
        return -1;
    }
    close(fd); //关闭文件
    return 0;
}

//开始捕获测试
int io_test( void )
{
	int fd = -1;
 
//导出io
	if(1 != gpio_export(42))
	{
		perror("Z_ACK  gpio_export failed!\n");  
	}
 
    //配置成输入、边沿捕获
    gpio_config(GPIO_42_PATH,"direction","in");
    gpio_config(GPIO_42_PATH,"edge","both");//配置成中断边沿捕获

 
    //开始捕获
    fd = open("/sys/class/gpio/gpio42/value",O_RDONLY); //todo： path优化
    if(fd<0)
    {
        perror("open Z_ACK failed!\n");  
        return -1;
    }
    else { perror("open Z_ACK Ok!\n");   }
 
 
    struct pollfd fds[1];
    fds[0].fd=fd;
    fds[0].events = POLLPRI;
 
    while(1)
    {
        if(poll(fds,1,0)==-1)
        {
            perror("poll failed!\n");
            return -1;
        }
 
        if( fds[0].revents & POLLPRI )
        {
    	    //set_ACK();  //能进这里就代表触发事件发生了,我这里是置了一个标志
            if(lseek(fd,0,SEEK_SET)==-1)
            {
                perror("lseek  Z_ACK value failed!\n");
                return -1;
            }
            char buffer[16];
            int len;
            //一定要读取,不然会一直检测到紧急事件
            if(( len = read(fd,buffer,sizeof(buffer))) == -1)  
            {
                perror("read Z_ACK value failed!\n");
                return -1;
            }
            buffer[len]=0;
            printf("READ GPIO_42 IS: %s\n",buffer);
        }
    }
 
	return 0;
}
/*

编译指令
/home/luke/openwrt/openwrt_19.07/staging_dir/toolchain-mipsel_24kc_gcc-7.5.0_musl/bin/mipsel-openwrt-linux-gcc io_interrupt_test.c -L /home/luke/openwrt/openwrt_19.07/staging_dir/target-mipsel_24kc_musl/usr/include -o io_interrupt_test
scp ./io_interrupt_test  root@192.168.0.131:/tmp/io_interrupt_test
 */
void main(void)
{
    io_test();
}