
/*

编译指令
/home/luke/openwrt/openwrt_19.07/staging_dir/toolchain-mipsel_24kc_gcc-7.5.0_musl/bin/mipsel-openwrt-linux-gcc endianness.c -L /home/luke/openwrt/openwrt_19.07/staging_dir/target-mipsel_24kc_musl/usr/include -o a
gcc endianness.c -L /home/luke/openwrt/openwrt_19.07/staging_dir/target-mipsel_24kc_musl/usr/include -o b

scp ./a  root@192.168.31.130:/tmp/a
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



/*
大小端序跟硬件的体系结构有关，所有x86系列的pc机都是小端序，跟操作系统无关。在x86系列的pc上的solaris系统是小端序，sun sparc平台的solaris是大端序。
    大端字节序，高字节存于内存低地址，低字节存于内存高地址；小端字节序反之。

     如一个long型数据0x12345678
       大端字节序：
        内存低地址--> 0x12
                        0x34
　　　　　　            0x56
        内存高地址--> 0x78

      小端字节序：
          内存低地址--> 0x78      
　　　　　　　        0x56
　　　　　　       　 0x34
          内存高地址--> 0x12

但是，当传输的数据以一个字节一个字节的方式进行发送传输的话，就没有大段序、小端序的问题了
 
判断CPU大小端的程序：

联合(union)方式判断法
在union中所有的数据成员共用一个空间，同一时间只能储存其中一个数据成员，所有的数据成员具有相同
的起始地址。即上述的union虽然定义了两个成员，但其实这个union只占用了4个字节(32位机器中)，往a成员
赋值，然后读取b就相读取a成员的低位第一个字节的值。如果机器使用大端模式，则u.a=1那a的最高字节值为1；
如果机器使用小段模式，则u.a=1则a的最低位字节为1。上述可知b和a有相同的起始位，所以读取b如果等于1，
则为小端模式，b为0则为大端模式。
*/

typedef union {  
    int a;  
    char b;  
}my_union;  
   
int checkSystem1(void)  
{  
    my_union u;  
    u.a = 1;  
    printf("u.b:%d\n",u.b);
    printf("u.a:%d\n",u.a);

    return (u.a == u.b);  
}  
int main(int argc, char *argv[])
{

    //大小端判断
    int a = checkSystem1();
    printf("endianness is %s\n", (a==1) ? "little":"big");

}
