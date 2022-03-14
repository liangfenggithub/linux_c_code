#include "stdio.h"
#include "string.h"

#include "time.h"

typedef unsigned long int uint64_t;
//https://www.runoob.com/w3cnote/c-time-func-summary.html
/**
 * @brief 时间字符串转时间戳
 * 
 * @param time_str 时间字符串,格式为:'2022-02-16 07:54:31'
 * @return timestamp 时间戳指针
 */
time_t timestr_to_time(char * time_str)
{
  struct tm stm;  
  memset(&stm, 0, sizeof(struct tm));
  strptime(time_str, "%Y-%m-%d %H:%M:%S",&stm);//：将以字符串形式表示的时间转为 tm 结构体表示的时间。
  time_t t = mktime(&stm);
  return t;
}
/**
 * @brief 时间戳转字符串
 * 
 * @param timestamp 时间戳
 * @param time_str 目标字符串
 */
 void time_to_timestr(time_t timestamp,char *time_str)
 {
    time_t time;
    struct tm *p;
    p = localtime(&timestamp);
    sprintf(time_str, "%d-%02d-%02d %02d:%02d:%02d",
            (1900 + p->tm_year),
            (1 + p->tm_mon),
            p->tm_mday,
            p->tm_hour,
            p->tm_min,
            p->tm_sec);
 }

int main(int argc, char *argv[])
{
    time_t timep;
    time_t timer[4];
    struct tm tm_1;
    struct tm *p;
    char time_str[40];

    // 1. 存储日期和时间
//     memset(time_str, '\0', 40);
//     time(&timep);
//     p = localtime(&timep);
//     sprintf(time_str, "%d-%02d-%02d %02d:%02d:%02d",
//             (1900 + p->tm_year),
//             (1 + p->tm_mon),
//             p->tm_mday,
//             p->tm_hour,
//             p->tm_min,
//             p->tm_sec);
//     printf("当前时间为:%s\n", time_str);
//     //当前时间为:2022-02-15 14:23:15

//     printf("asctime 函数:%s\n",asctime(p));//asctime将结构struct tm * ptr所表示的时间以字符串表示
//     //asctime 函数:Tue Feb 15 14:23:15 2022

//     // 2. 存储日历时间 距离UTC时间的秒数
//     time_t seconds;
//     seconds = time(NULL);
//     printf("自 1970-01-01 起的小时数 = %ld, 秒数:%ld\n", seconds / 3600, seconds);
//     //自 1970-01-01 起的小时数 = 456926, 秒数:1644934995

//     //3. 计算时间查 单位为秒
//     printf("\n 3. 计算时间查 单位为秒\n");

//     time_t first,second;  
//     time(&first);  
//     sleep(2);  
//     time(&second);
//     printf("时间查为: %f 秒\n",difftime(second,first));  //difftime 得到两次机器时间差，单位为秒

//     /*注意:
//     通过自定义的指针（struct tm *）对tm结构体成员的引用不包含赋值操作，除非另外定义tm结构体变量。
//     例如：
//     struct tm *p={0};
//     之类的赋值操作都是非法的。
//     在linux下，通过自定义指针对tm结构体成员赋值操作编译可以通过，但执行会提示段错误Segmentation fault(coredump)；在gdb模式下，会得到“Cannot access memory at address XXX”的警告。
//     如果要利用tm结构体成员保存时间数据，可以另外定义struct tm类型的变量。
//     例如：struct tm t;
//     t.tm_hour=14;
//     p->tm_hour=14;
//     如上的操作是合法的
//     */

//    //4. time_t和struct tm之间的转换
//     int i;
//     printf("\n 4. time_t和struct tm之间的转换\n");
//     tm_1.tm_year = 2015;
//     tm_1.tm_mon = 2;
//     tm_1.tm_mday = 23;
//     tm_1.tm_hour = 8;
//     tm_1.tm_min = 22;
//     tm_1.tm_sec = 0;
//     for(int i = 0; i < 4; i++){
        
//         timer[i] = mktime(&tm_1);
//         printf ("timer[%d] = %ld\n", i, timer[i]);

//         p = localtime(&timer[i]);
//         printf("%d-%d-%d-%d-%d-%d\n",
//                 p->tm_year,
//                 p->tm_mon,
//                 p->tm_mday,
//                 p->tm_hour,
//                 p->tm_min,
//                 p->tm_sec);
//     }


    //5. 字符串转时间戳
    printf("\n 5. 字符串转时间戳\n");

    char now[50] = "2022-02-24 11:39:00";
    time_t now_stamp = timestr_to_time(now);
    printf("字符串:%s 对应的时间戳为:%d",now,now_stamp);
    

    //6. 时间戳转字符串
    printf("\n 6. 时间戳转字符串\n");
    memset(time_str, '\0', 40);
    time_to_timestr(now_stamp,time_str);
    printf("时间戳:%d 对应的字符串结果为:%s\n",now_stamp, time_str);


    //7.用time()取得时间 (秒数), 利用localtime() 转换成struct tm 再利用mktine()将structtm 转换成原来的秒数。
/*
    printf("\n 用time()取得时间 (秒数), 利用localtime() 转换成struct tm 再利用mktine()将structtm 转换成原来的秒数\n");
    time(&timep);
    printf("time() : %d \n", timep);
    p = localtime(&timep);
    memset(time_str, '\0', 40);
    sprintf(time_str, "%d-%02d-%02d %02d:%02d:%02d",
            (1900 + p->tm_year),
            (1 + p->tm_mon),
            p->tm_mday,
            p->tm_hour,
            p->tm_min,
            p->tm_sec);
    printf("p:%s\n", time_str);
    timep = mktime(p);
    printf("time()->localtime()->mktime():%d\n", timep);
*/
}
