#if 1

#include <sys/inotify.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

void main(void)
{
  char data[16384] = {0};
  long int readLength = 0;
  char *eventptr;
  int step = 0;
  struct inotify_event event;

  int need_parse = 0;
  int need_remove_wd = 0;
  int watch_fd;
  int watch_fd_list;

  while (1)
  {
    watch_fd = inotify_init();
    // 注意文件必须为全路径
    // watch_fd_list = inotify_add_watch(watch_fd, "/home/luke/code/linux_c_code/inotify/build/bin/aaa", IN_ALL_EVENTS); //监听文件是否打开
    watch_fd_list = inotify_add_watch(watch_fd, "/etc/config/ptl_config", IN_ALL_EVENTS); //监听文件是否打开
    // watch_fd_list = inotify_add_watch(watch_fd, "/home/luke/code/linux_c_code/inotify/src/aaa", IN_ALL_EVENTS); //监听文件是否打开



   printf( "begin read watch_config\n");
    readLength = read(watch_fd, data, 16384);
   printf( "end read watch_config,read readLength is %d\n", readLength);

    eventptr = data;
    while (readLength > 0)
    {
      event.wd = *((int *)eventptr);
      step += (int)sizeof(int);
      // eventptr += sizeof(int);
      event.mask = *((uint32_t *)(eventptr + step));
      step += (int)sizeof(uint32_t);
      event.cookie = *((uint32_t *)(eventptr + step));
      step += (int)sizeof(uint32_t);
      event.len = *((uint32_t *)(eventptr + step));
      step += (int)sizeof(uint32_t);
      step = 0;

      if (event.mask & IN_MODIFY)
      {
       printf( "Event Config File was Changed \n ");
        need_parse = 1;
      }
      else if (event.mask & IN_CREATE)
      {
       printf( "Event File was CREATE \n");
        need_parse = 1;
      }
      else if (event.mask & (IN_DELETE | IN_DELETE_SELF))
      {
       printf( "Event Config File was DELETE\n");
        need_parse = 1;
        need_remove_wd = 1;
        //重新解析配置
      }
      else if (event.mask & (IN_MOVE | IN_MOVE_SELF | IN_MOVED_TO | IN_MOVED_FROM))
      {
       printf( "Event file was moved \n");
        need_parse = 1;
        need_remove_wd = 1;
      }
      else if (event.mask & IN_CLOSE_WRITE)
      {
       printf( "Event file was close by write \n");
        // need_parse = 1;
      }
      else if (event.mask & IN_ACCESS)
      {
        //printf("file IN_ACCESS \n");
      }
      else if (event.mask & IN_ATTRIB)
      {
        need_parse = 1;
        need_remove_wd = 1;
        // IN_ATTRIB：元数据被改变，例如权限、时间戳、扩展属性、链接数、UID、GID等
       printf( "Event file IN_ATTRIB \n");
      }
      else if (event.mask & IN_OPEN)
      {
        //printf("file IN_OPEN \n");
      }
      else if (event.mask & IN_CLOSE_NOWRITE)
      {
        //printf("file IN_CLOSE_NOWRITE \n");
      }
      else if (event.mask & IN_IGNORED)
      {
        //printf("file IN_IGNORED \n");
      }
      else if (event.mask & IN_ISDIR)
      {
        //printf("file IN_ISDIR \n");
      }
      else if (event.mask & IN_Q_OVERFLOW)
      {
        //printf("file IN_Q_OVERFLOW \n");
      }
      else if (event.mask & IN_UNMOUNT)
      {
        //printf("file IN_UNMOUNT \n");
      }
      else
      {
        //printf("event.mask:%d", event.mask);
      }

      eventptr += sizeof(struct inotify_event) + event.len;
      readLength = readLength - (long int)(sizeof(struct inotify_event) + event.len);
    }

    if (need_parse)
    {
    //  printf( "need_parse\n");

      // GetPTLConfig();
    }

    if (need_remove_wd)
    {
    //  printf( "need_remove_wd\n");

      // eloop_unregister_read_sock(EventFileLis);

      // rm_inotify_wd(EventFileLis, wdEventFileLis);

      // wdEventFileLis = inotify_add_watch(EventFileLis, "/etc/config/PTLConfigback", IN_ALL_EVENTS); //监听文件是否被改变

      // if (wdEventFileLis < 0)
      // {
      //     RFID_log_error("Listion Confile File error. Reason:%s \n", strerror(errno));
      //     return;
      // }
      // else
      // {
      //     //gs_device_config.fd_config_listen = fd_config_listen;
      //     //LOG_printf(LOG_ALWAYS,"fd_config_listen %d ========\n", fd_config_listen);
      //     //监听客户端连接
      //     eloop_register_read_sock(EventFileLis, Event_handle_config_file_changed, NULL, NULL);
      // }
    }
  }
}

// /*
// struct inotify_event {
//    int      wd;       // Watch descriptor
//    uint32_t mask;     // Mask of events
//    uint32_t cookie;   // Unique cookie associating related  events (for rename(2))
//    uint32_t len;      // Size of name field
//    char     name[];   // Optional null-terminated name
// };
// */

// int watch_inotify_events(int fd)
// {
//     char event_buf[512];
//     int ret;
//     int event_pos = 0;
//     int event_size = 0;
//     struct inotify_event *event;

//     /*读事件是否发生，没有发生就会阻塞*/
//     ret = read(fd, event_buf, sizeof(event_buf));

//     /*如果read的返回值，小于inotify_event大小出现错误*/
//     if(ret < (int)sizeof(struct inotify_event))
//     {
//         printf("counld not get event!\n");
//         return -1;
//     }

//     /*因为read的返回值存在一个或者多个inotify_event对象，需要一个一个取出来处理*/
//     while( ret >= (int)sizeof(struct inotify_event) )
//     {
//         event = (struct inotify_event*)(event_buf + event_pos);
//         if(event->len)
//         {
//             if(event->mask & IN_CREATE)
//             {
//                 printf("create file: %s\n",event->name);
//             }
//             else
//             {
//                 printf("delete file: %s\n",event->name);
//             }
//         }

//         /*event_size就是一个事件的真正大小*/
//         event_size = sizeof(struct inotify_event) + event->len;
//         ret -= event_size;
//         event_pos += event_size;
//     }

//     return 0;
// }

// int main(int argc, char** argv)
// {
//     int InotifyFd;
//     int ret;

//     if (argc != 2)
//     {
//         printf("Usage: %s <dir>\n", argv[0]);
//         return -1;
//     }

//     /*inotify初始化*/
//     InotifyFd = inotify_init();
//     if( InotifyFd == -1)
//     {
//         printf("inotify_init error!\n");
//         return -1;
//     }

//     /*添加watch对象*/
//     ret = inotify_add_watch(InotifyFd, argv[1], IN_CREATE |  IN_DELETE);

//     /*处理事件*/
//     watch_inotify_events(InotifyFd);

//     /*删除inotify的watch对象*/
//     if ( inotify_rm_watch(InotifyFd, ret) == -1)
//     {
//         printf("notify_rm_watch error!\n");
//         return -1;
//     }

//     /*关闭inotify描述符*/
//     close(InotifyFd);

//     return 0;
// }

#else

#include <sys/epoll.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/inotify.h>

#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (1024 * (EVENT_SIZE + 16))

int main(int argc, char *argv[])
{
  int i, epfd, nfds, fd;
  int wd;
  int length;

  char buffer[BUF_LEN];
  struct epoll_event ev, events[20];
  epfd = epoll_create(256);
  fd = inotify_init();
  wd = inotify_add_watch(fd, "/home/cc/tmp",
                         IN_MODIFY | IN_CREATE | IN_DELETE);

  ev.data.fd = fd;
  ev.events = EPOLLIN | EPOLLET;

  epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);

  for (;;)
  {
    nfds = epoll_wait(epfd, events, 20, 500);

    for (i = 0; i < nfds; ++i)
    {
      if (events[i].data.fd == fd)

      {
        length = read(fd, buffer, BUF_LEN);

        if (length < 0)
        {
          perror("read");
        }

        while (i < length)
        {
          struct inotify_event *event =
              (struct inotify_event *)&buffer[i];
          if (event->len)
          {
            if (event->mask & IN_CREATE)
            {
              if (event->mask & IN_ISDIR)
              {
                printf("The directory %s was created.\n",
                       event->name);
              }
              else
              {
                printf("The file %s was created.\n",
                       event->name);
              }
            }
            else if (event->mask & IN_DELETE)
            {
              if (event->mask & IN_ISDIR)
              {
                printf("The directory %s was deleted.\n",
                       event->name);
              }
              else
              {
                printf("The file %s was deleted.\n",
                       event->name);
              }
            }
            else if (event->mask & IN_MODIFY)
            {
              if (event->mask & IN_ISDIR)
              {
                printf("The directory %s was modified.\n",
                       event->name);
              }
              else
              {
                printf("The file %s was modified.\n",
                       event->name);
              }
            }
          }
          i += EVENT_SIZE + event->len;
        }
      }
    }
  }

  return 0;
}
#endif