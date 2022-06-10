#include "stdio.h"
#include "string.h"
// #include <sys/stat.h>
#include <dirent.h>
#define FILE_DEBUG_PATH "/home/luke/code/linux_c_code"

struct dirent *dirent;
DIR *dir;

int main(int argc, char *argv[])
{
    //打开目录
    // 用法：opendir函数打开一个与目录名name相对应的目录流，如果成功，返回指向这个目录流的指针，并且指向的是这个目录流中第一个目录项；如果失败，返回NULL；
    if ((dir = opendir(FILE_DEBUG_PATH)) == NULL)
    {
        printf("Open dir %s of error...", FILE_DEBUG_PATH);
        return -1;
    }

    //遍历目录下的文件

    // readdir 返回一个结构体dirent，代表了由dir指向的目录流中的下一个目录项，如果读到end-of-fiie或者出现错误，则返回NULL;
    // 结构体如下：
    // struct dirent { 
    //     ino_t          d_ino;       /* inode number索引节点号 */ 
    //     off_t          d_off;       /* offset to the next dirent 在目录文件中的偏移 */ 
    //     unsigned short d_reclen;    /* length of this record 文件名长 */ 
    //     unsigned char  d_type;      /* type of file 文件类型  */ 
    //     char           d_name[256]; /* filename */ 
    // };
    while ((dirent = readdir(dir)) != NULL)
    {
        printf("name: %s\n",dirent->d_name);
        // if (strncmp(ptr->d_name, "PTL_debug_", strlen("PTL_debug_")) == 0)
        // {
        //     sscanf(ptr->d_name, "%*[^_]_%*[^_]_%d", &gs_log_file.date[file_num]);
        //     if (++file_num >= FILE_DEBUG_NUM)
        //         break;
        // }
        // else
        // {
        //     continue;
        // }
    }
    // 关闭目录流dir，成功，返回0，失败返回NULL;
    closedir(dir);
}
