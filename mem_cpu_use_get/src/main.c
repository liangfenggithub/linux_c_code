#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>
int main()
{
    // 打开/proc/stat文件，读取CPU的使用情况
    FILE *fp = fopen("/proc/stat", "r");
    if (fp == NULL)
    {
        perror("Failed to open /proc/stat");
        return 1;
    }

    // 读取第一行，获取总的CPU时间
    // cpu  21592 8764 18729 4051229 1073 0 3591 0 0 0
    unsigned long long user, nice, system, idle;
    if (fscanf(fp, "cpu %llu %llu %llu %llu", &user, &nice, &system, &idle) != 4)
    {
        perror("Failed to read /proc/stat");
        return 1;
    }
    fclose(fp);

    // 计算总的CPU时间
    unsigned long long total = user + nice + system + idle;

    // 打开/proc/meminfo文件，读取内存的使用情况
    fp = fopen("/proc/meminfo", "r");
    if (fp == NULL)
    {
        perror("Failed to open /proc/meminfo");
        return 1;
    }

    // 读取第一、二行，获取总内存和空闲内存
    //   MemTotal:        1999332 kB
    //   MemFree:          361972 kB
    unsigned long long total_memory, free_memory;
    if (fscanf(fp, "MemTotal: %llu kB\nMemFree: %llu kB\n", &total_memory, &free_memory) != 2)
    {
        perror("Failed to read /proc/meminfo");
        return 1;
    }
    fclose(fp);

    // 计算本机内存和CPU的使用率并输出
    double memory_usage = (total_memory - free_memory) * 100.0 / total_memory;
    double cpu_usage = 100.0 - idle * 100.0 / total;
    // 打印CPU和内存的使用情况
    // CPU: 1.69%
    // MEM: 96.32%
    printf("CPU: %.2f%%\n", cpu_usage);
    printf("MEM: %.2f%%\n", memory_usage);


    //获取本应用进程资源使用个情况
    // struct rusage usage;
    // //调用 getrusage() 函数时，需要指定要获取信息的进程，这里我们使用 RUSAGE_SELF 常量来获取当前进程的信息。
    // getrusage(RUSAGE_SELF, &usage);
    // // 在调用 getrusage() 函数之后，我们可以通过访问 ru_utime 和 ru_maxrss 字段来获取 CPU 使用时间和内存使用量。
    // printf("CPU usage: %ld.%06ld sec\n", usage.ru_utime.tv_sec, usage.ru_utime.tv_usec);
    // printf("Memory usage: %ld kB\n", usage.ru_maxrss);
    // CPU usage: 0.004779 sec
    // Memory usage: 22932 kB



    // 获取当前进程的资源使用情况
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);

    // 计算 CPU 利用率（单位：百分比）
    double cpu_percent = (double)(usage.ru_utime.tv_sec + usage.ru_stime.tv_sec) /(double)(sysconf(_SC_CLK_TCK));
    printf("CPU utilization: %.2f%%\n", cpu_percent * 100);

    // 获取内存使用情况（单位：字节）
    long memory_usage_self = usage.ru_maxrss * 1024;
    printf("Memory usage: %ld bytes\n", memory_usage_self);
}