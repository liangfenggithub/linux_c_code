#include "stdio.h"
#include "time.h"

typedef unsigned long int uint64_t;
//https://www.runoob.com/w3cnote/c-time-func-summary.html
int main(int argc, char *argv[])
{
    time_t timep;
    struct tm *p;
    char time_str[40];

    // 1. 存储日期和时间
    memset(time_str, '\0', 40);
    time(&timep);
    p = localtime(&timep);
    sprintf(time_str, "%d-%02d-%02d %02d:%02d:%02d",
            (1900 + p->tm_year),
            (1 + p->tm_mon),
            p->tm_mday,
            p->tm_hour,
            p->tm_min,
            p->tm_sec);
    printf("当前时间为:%s\n", time_str);
    //当前时间为:2022-02-15 14:23:15

    printf("asctime 函数:%s\n",asctime(p));//asctime将结构struct tm * ptr所表示的时间以字符串表示
    //asctime 函数:Tue Feb 15 14:23:15 2022

    // 2. 存储日历时间 距离UTC时间的秒数
    time_t seconds;
    seconds = time(NULL);
    printf("自 1970-01-01 起的小时数 = %ld, 秒数:%ld\n", seconds / 3600, seconds);
    //自 1970-01-01 起的小时数 = 456926, 秒数:1644934995

    //3 计算时间查 单位为秒
    time_t first,second;  
    time(&first);  
    sleep(2);  
    time(&second);
    printf("时间查为: %f 秒\n",difftime(second,first));  //difftime 得到两次机器时间差，单位为秒

    /*注意:
    通过自定义的指针（struct tm *）对tm结构体成员的引用不包含赋值操作，除非另外定义tm结构体变量。
    例如：
    struct tm *p={0};
    之类的赋值操作都是非法的。
    在linux下，通过自定义指针对tm结构体成员赋值操作编译可以通过，但执行会提示段错误Segmentation fault(coredump)；在gdb模式下，会得到“Cannot access memory at address XXX”的警告。
    如果要利用tm结构体成员保存时间数据，可以另外定义struct tm类型的变量。
    例如：struct tm t;
    t.tm_hour=14;
    p->tm_hour=14;
    如上的操作是合法的
    */
}
