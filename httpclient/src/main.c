#include "./http.h"
#include "stdio.h"
struct ft_http_client_t;
int main()
{
    ft_http_client_t *http = 0;

    ft_http_init();

    http = ft_http_new();

    while (1)
    {
        char ch = getchar();

        if (ch == -1)
        {
            usleep(100 * 1000);
            continue;
        }
        if (ch == 'q')
            break;

        switch (ch)
        {
        case 'a':
        {
            const char *body = 0;
            // ft_http_set_timeout(http, 4);

            body = ft_http_sync_request(http, "http://www.fengfengiot.cn/", M_GET);

            printf("ft_http get status is %d\n",http->status_code);//为什么不可呢？？
            // if (http->status_code == 200)
            // {

            //     // printf("ft_http get:%s\n", body);
            // }
            // printf("ft_http get:%s\n", body);
        }
        break;
        case 'd':
        {
            int code = 0;

            // ft_http_set_timeout(http, 10*1000);

            code = ft_http_sync_download_file(http, "http://xxx.com/42b24b57eaaa.jpg", "code.jpg");
            printf("ft_http get:%d\n", code);
        }
        break;
        }
    }

    if (http)
    {
        ft_http_destroy(http);
    }
    ft_http_deinit();
}
