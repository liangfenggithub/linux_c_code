#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <fcntl.h>
#include "pthread.h"

// posix消息队列注意事项
// * Linux系统中提供了POSIX和 System V这两种不同的接口，POSIX为可移植的操作系统接口。System V 是 AT&T 的第一个商业UNIX版本(UNIX System III)的加强。
// * attr.mq_maxmsg 不能超过文件 /proc/sys/fs/mqueue/msg_max 中的数值，我的机器上面是10。
// * attr.mq_msgsize不能超过 /proc/sys/fs/mqueue/msgsize_max 的数值。
// * 消息队列名称前面必须加上斜杆,且之后名称不能有斜杠
// * 多次携带O_CREAT的open操作失败，因为第一次已经创建了，第二次不能在进行创建，因此可以每次创建前先unlink也就是执行删除消息队列一次
// * 查看队列可以执行 ls /dev/mqueue
// * 消息队列在程序退出后不会删除，除非利用unlink或者内核重新自举后删除，重新自举应该就是重启
// * mq_receive() 的第三个参数表示读取消息的长度，不能小于能写入队列中消息的最大大小，即一定要大于等于该队列的 mq_attr 结构中 mq_msgsize 的大小。
// * 消息的优先级：它是一个小于 MQ_PRIO_MAX 的数，数值越大，优先级越高。 POSIX 消息队列在调用 mq_receive 时总是返回队列中最高优先级的最早消息。如果消息不需要设定优先级，那么可以在 mq_send 是置 msg_prio 为 0， mq_receive 的 msg_prio 置为 NULL。
// * 编译需要选择rt库：-lrt ，-l 指的是链接库文件，rt为库的名字rt。也即是librt.a/librt.so
// * 默认情况下mq_send和mq_receive是阻塞进行调用，可以通过mq_setattr来设置为O_NONBLOCK，如：
//       struct mq_attr new_attr;
//       mq_getattr(mqID, &new_attr);//获取当前属性
//       new_attr.mq_flags = O_NONBLOCK;//设置为非阻塞
//       mq_setattr(mqID, &new_attr, NULL)//设置属性

/*如果在mips上测试 编译指令

编译指令
cd src
/home/luke/openwrt/openwrt_19.07/staging_dir/toolchain-mipsel_24kc_gcc-7.5.0_musl/bin/mipsel-openwrt-linux-gcc main.c -L /home/luke/openwrt/openwrt_19.07/staging_dir/target-mipsel_24kc_musl/usr/include -o mq_test
scp ./mq_test  root@192.168.0.130:/tmp/mq_test

*/
#define MQ_NAME "/up_to_mqtt_324"
mqd_t out_to_tcp_client_mq;
pthread_t tid;
// pthread_attr_t attr;

typedef struct
{
    int id;
    char info[30]
} my_msg_t;

void *thread_mq_rcv(void *arg)
{
    int nbytes = 0;
    my_msg_t msg_rcv;
    printf("enter thread_mq_rcv\n");
    while (1)
    {
        printf("begin mq_receive\n");
        memset(&msg_rcv,0,sizeof(my_msg_t));
        nbytes = mq_receive(out_to_tcp_client_mq, (char *)(&msg_rcv), sizeof(my_msg_t), NULL); //这里的20, 表示长度大于或等于mq_msgsize，否则返回 -1
        if (nbytes < 0)
        {
            printf("mq_receive error: %d \n", nbytes);
            return;
        }
        else
        {
            printf("mq_receive byte is: %d \n", nbytes);

            printf("mq_receive info: %s\n", msg_rcv.info);
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    my_msg_t msg_send;


    //0. 输入参数检测
    if(argc<2)
    {
        printf("please input mq_maxmsg \n");
        return;
    }

    // 1. 设置队列属性
    struct mq_attr attr;
    attr.mq_maxmsg = atoi(argv[1]); //这里的大小受系统限制
    printf("you set mq_maxmsg is %d\n",attr.mq_maxmsg);
    attr.mq_msgsize = sizeof(my_msg_t);

    // 2. 删除、创建、打开队列
    printf("mq_unlink res:%d\n", mq_unlink(MQ_NAME));
    out_to_tcp_client_mq = mq_open(MQ_NAME, O_CREAT | O_RDWR | O_EXCL, 0666, &attr);

    printf("%d\n", out_to_tcp_client_mq);
    if (out_to_tcp_client_mq < 0)
    {
        printf("open mq error:%d\n", out_to_tcp_client_mq);
        return;
    }
    else
    {
        printf("open mq success\n");
    }

    // 3. 创建接收线程
    //  pthread_attr_init(&attr);
    int errno = pthread_create(&tid, NULL, thread_mq_rcv, (void *)"2");
    if (errno != 0)
    {
        perror("create thread failed\n");
        return -1;
    }
    sleep(5);


    // 4. 向队列发送数据
    for (int i = 0; i < 5; i++)
    {
        msg_send.id = i;
        sprintf(&(msg_send.info), "test_%d", i);

        int res = mq_send(out_to_tcp_client_mq,(char *)&msg_send,sizeof(my_msg_t),NULL);
        if (res < 0){
            printf("mq_send error: %d \n", res);
            return;
        }else{
            printf("mq_send success: %d \n", res);
        }

        //发送一条等于0的消息试试,看能否打破阻塞
        mq_send(out_to_tcp_client_mq,NULL,0,NULL);
    }

    


    pthread_join(tid, NULL);

    // 5. 关闭队列
    if (mq_close(out_to_tcp_client_mq) < 0)
    {
        printf("close mq error\n");
    }
    else
    {
        printf("close mq success\n");
    }
}