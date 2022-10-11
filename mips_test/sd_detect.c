
/*
SD卡热插拔检测，在openwrt中好用！
具体工作原理参考
https://blog.csdn.net/ganshuyu/article/details/30241313

编译指令
/home/luke/openwrt/openwrt_19.07/staging_dir/toolchain-mipsel_24kc_gcc-7.5.0_musl/bin/mipsel-openwrt-linux-gcc sd_detect.c -L /home/luke/openwrt/openwrt_19.07/staging_dir/target-mipsel_24kc_musl/usr/include -o a
gcc sd_detect.c -L /home/luke/openwrt/openwrt_19.07/staging_dir/target-mipsel_24kc_musl/usr/include -o b

scp ./a  root@192.168.0.131:/tmp/a
scp ./a  root@192.168.31.130:/tmp/a

a 40 1
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <asm/types.h>
//该头文件需要放在netlink.h前面防止编译出现__kernel_sa_family未定义
#include <sys/socket.h>
#include <linux/netlink.h>
#include <unistd.h>
 
int monitorNetlinkUevent()
{
        int sockfd;
    struct sockaddr_nl sa;
    int len;
    char buf[4096];
    struct iovec iov;
    struct msghdr msg;
    int i;
 
    memset(&sa, 0, sizeof(sa));
    sa.nl_family = AF_NETLINK;
    sa.nl_groups = NETLINK_KOBJECT_UEVENT;
    sa.nl_pid = 0; //getpid(); both is ok
    memset(&msg, 0, sizeof(msg));
    iov.iov_base = (void *)buf;
    iov.iov_len = sizeof(buf);
    msg.msg_name = (void *)&sa;
    msg.msg_namelen = sizeof(sa);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
 
    sockfd = socket(AF_NETLINK, SOCK_RAW, NETLINK_KOBJECT_UEVENT);
    if (sockfd == -1)
        printf("socket creating failed:%s\n", strerror(errno));
    if (bind(sockfd, (struct sockaddr *)&sa, sizeof(sa)) == -1)
        printf("bind error:%s\n", strerror(errno));
 
    while (1)
    {
        len = recvmsg(sockfd, &msg, 0); //是阻塞吗？
        if (len < 0)
            printf("receive error\n");
        else if (len < 32 || len > sizeof(buf))
            printf("invalid message");
        for (i = 0; i < len; i++)
            if (*(buf + i) == '\0')
                buf[i] = '\n';
        // printf("received %d bytes\n%s\n", len, buf);

        if(strstr(buf, "ACTION=add") != 0 && strstr(buf, "SUBSYSTEM=mmc") && strstr(buf, "MMC_TYPE=SD") != 0)
        {
            printf("插入SD卡\n");
        }
        else if (strstr(buf, "ACTION=bind") != 0 && strstr(buf, "SUBSYSTEM=mmc") && strstr(buf, "MMC_TYPE=SD") != 0)
        {
            printf("挂载SD卡成功\n");

        }
        else if (strstr(buf, "ACTION=remove") != 0 && strstr(buf, "SUBSYSTEM=mmc") && strstr(buf, "MMC_TYPE=SD") != 0)
        {
            printf("拔掉SD卡\n");
        }
    }
}
 
int main(int argc, char **argv)
{
    monitorNetlinkUevent();
    return 0;
}