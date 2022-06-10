/*

编译指令
/home/luke/openwrt/openwrt_19.07/staging_dir/toolchain-mipsel_24kc_gcc-7.5.0_musl/bin/mipsel-openwrt-linux-gcc io_test_speed.c -L /home/luke/openwrt/openwrt_19.07/staging_dir/target-mipsel_24kc_musl/usr/include -o a
scp ./a  root@192.168.31.130:/tmp/a

a 40 1
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
static char gpio_path[100];
static int gpio_config(const char *attr, const char *val)
{
    char file_path[100];
    int len;
    int fd;
    sprintf(file_path, "%s/%s", gpio_path, attr);
    if (0 > (fd = open(file_path, O_WRONLY)))
    {
        perror("open error");
        return fd;
    }
    len = strlen(val);
    if (len != write(fd, val, len))
    {
        perror("write error");
        close(fd);
        return -1;
    }
    close(fd); //关闭文件
    return 0;
}

static int gpio_change(int delay_us)
{
    char file_path[100];
    int len;
    int fd;
    sprintf(file_path, "%s/value", gpio_path);
    if (0 > (fd = open(file_path, O_WRONLY)))
    {
        perror("open error");
        return fd;
    }
    while(1)
    {
        if (1 != write(fd, "1", 1))
        {
            perror("write 1 error");
            close(fd);
            return -1;
        }
        usleep(delay_us);

        if (1 != write(fd, "0", 1))
        {
            perror("write 1 error");
            close(fd);
            return -1;
        }
        usleep(delay_us);
    }

    close(fd); //关闭文件
    return 0;
}
int main(int argc, char *argv[])
{

    /* 校验传参 */
    if (2 != argc)
    {
        fprintf(stderr, "usage: %s <gpio>\n", argv[0]);
        exit(-1);
    }
    /* 判断指定编号的 GPIO 是否导出 */
    sprintf(gpio_path, "/sys/class/gpio/gpio%s", argv[1]);
    printf("gpio path is %s\n",gpio_path);
    if (access(gpio_path, F_OK))
    { //如果目录不存在 则需要导出
        printf("gpio path need export %s\n",gpio_path);

        int fd;
        int len;
        if (0 > (fd = open("/sys/class/gpio/export", O_WRONLY)))
        {
            perror("open error");
            exit(-1);
        }

        //导出 gpio
        len = strlen(argv[1]);
        if (len != write(fd, argv[1], len))
        { 
            perror("write error");
            close(fd);
            exit(-1);
        }
        close(fd); //关闭文件
    }
    else
    {
        printf("gpio path don't need export %s\n",gpio_path);
    }

    /* 配置为输出模式 */
    if (gpio_config("direction", "out"))
        exit(-1);
    /* 极性设置 */
    if (gpio_config("active_low", "0"))
        exit(-1);


#if 0
    while(1)
    {
        /* 控制 GPIO 输出高低电平 */
        if (gpio_config("value", "1"))
            exit(-1);
        usleep(200);
        if (gpio_config("value", "0"))
            exit(-1);
        usleep(200);
        // 每次设置输入输出都执行打开关闭操作，这种方式翻转电平
        // 设置400ms周期，实际输出最小周期位400ms cpu占 0%
        // 设置400us周期，实际输出最小周期位766us cpu占 29%
        // 设2us周期，实际输出最小周期为352us cpu占71%
        // 设翻转之间无延时 最小周期149us cpu占 100%
    }
#else
        gpio_change(1);

        // 每次设置输入输出都执行打开关闭操作，这种方式翻转电平
        // 设置400us周期，实际输出最小周期位528us cpu占 4%
        // 设2us周期，实际输出最小周期为131us cpu占20%
        // 设翻转之间无延时 最小周期5.8us cpu占 99%


        // 说明打开关闭操作很占cpu资源！！！
#endif

    /* 退出程序 */
    exit(0);
}