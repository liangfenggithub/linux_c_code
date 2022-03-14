

#include "libwebsockets.h"
#include <signal.h>
#include <string.h>

#define bool char

volatile int exit_sig = 0;

#define MAX_PAYLOAD_SIZE 1 * 1024

void sighdl(int sig)
{
    lwsl_notice("%d traped", sig);
    exit_sig = 1;
}

int client_index = 0;

/*
 * One of these is created for each client connecting to us.
 *
 * It is ONLY read or written from the lws service thread context.
 */
struct per_session_data__minimal
{
    struct per_session_data__minimal *pss_list;
    struct lws *wsi;

    char name[20];
    uint32_t tail;
    char buf[LWS_PRE + MAX_PAYLOAD_SIZE];
    int len;

    bool bin;
    bool fin;
};

/* one of these is created for each vhost our protocol is used with */

struct per_vhost_data__minimal
{
    struct lws_context *context;
    struct lws_vhost *vhost;
    const struct lws_protocols *protocol;

    struct per_session_data__minimal *pss_list; /* linked-list of live pss*/

    char finished;
};

static int protocol_ws_callback(struct lws *wsi, enum lws_callback_reasons reason,
                                void *user, void *in, size_t len)
{

    // struct session_data *data = (struct session_data *) user;
    struct per_session_data__minimal *pss =
        (struct per_session_data__minimal *)user;

    struct per_vhost_data__minimal *vhd =
        (struct per_vhost_data__minimal *)
            lws_protocol_vh_priv_get(lws_get_vhost(wsi),
                                     lws_get_protocol(wsi));
    int n, m, r = 0;

    switch (reason)
    {
    case LWS_CALLBACK_PROTOCOL_INIT: //建立链接时的初始化
        /* create our per-vhost struct */
        vhd = lws_protocol_vh_priv_zalloc(lws_get_vhost(wsi),
                                          lws_get_protocol(wsi),
                                          sizeof(struct per_vhost_data__minimal));
        if (!vhd)
        {
            return 1;
        }

        vhd->context = lws_get_context(wsi);
        vhd->protocol = lws_get_protocol(wsi);
        vhd->vhost = lws_get_vhost(wsi);

        // TODO:创建或者初始化接收缓存区!

        break;
    case LWS_CALLBACK_ESTABLISHED: // 当服务器和客户端完成握手后
        /* add ourselves to the list of live pss held in the vhd */
        sprintf(pss->name, "client_%d", client_index++);
        printf("Client connect,create new name %s\n", pss->name);

        //将链接session 加入链表中
        lws_ll_fwd_insert(pss, pss_list, vhd->pss_list);
        // pss->tail = lws_ring_get_oldest_tail(vhd->ring);
        pss->wsi = wsi;
        break;
    case LWS_CALLBACK_CLOSED: //链接关闭
                              /* remove our closing pss from the list of live pss */
        printf("%s: closed\n", pss->name);
        //从已连接链表中删除关闭的链接session
        lws_ll_fwd_remove(struct per_session_data__minimal, pss_list,
                          pss, vhd->pss_list);
        break;
    case LWS_CALLBACK_RECEIVE: // 当接收到客户端发来的帧以后
        printf("%s rcv msg, len is: %d", pss->name,len);
        // 判断是否最后一帧
        pss->fin = lws_is_final_fragment(wsi);
        printf(pss->fin ? "end is true," : "end is false,");
        // 判断是否二进制消息
        pss->bin = lws_frame_is_binary(wsi);
        printf(pss->bin ? "bin is true\n" : "bin is false\n");

        // 对服务器的接收端进行流量控制，如果来不及处理，可以控制之
        // 下面的调用禁止在此连接上接收数据
        // lws_rx_flow_control(wsi, 0);

        // 业务处理部分，为了实现Echo服务器，把客户端数据保存起来
        memcpy(&pss->buf[LWS_PRE], in, len);
        pss->len = len;
        printf("%s: recvied message:%s\n", pss->name, &pss->buf[LWS_PRE]);

        //判断是否为完整一帧数据,如果是,那么解码执行并返回
        if(pss->fin)
        {
            //Json反序列化,提取有用信息

        }

        // 需要给客户端应答时，触发一次写回调
        lws_callback_on_writable(wsi); //将ws连接加入可写事件监听
        break;
    case LWS_CALLBACK_SERVER_WRITEABLE: // 当此连接可写时

        m = lws_write(wsi, &pss->buf[LWS_PRE], pss->len, LWS_WRITE_TEXT);
        // m = lws_write(wsi, &pss->buf[LWS_PRE], pss->len, LWS_WRITE_BINARY);

        if (m < (int)pss->len)
        {
            printf("%s: ERROR %d writing to ws socket\n", pss->name, m);
            return -1;
        }

        // /* more to do? */
        // if (lws_ring_get_element(vhd->ring, &pss->tail))
        // 	/* come back as soon as we can write more */
        // 	lws_callback_on_writable(pss->wsi);

        // 下面的调用允许在此连接上接收数据
        // lws_rx_flow_control(wsi, 1);
        break;

    case LWS_CALLBACK_EVENT_WAIT_CANCELLED:
        if (!vhd)
            break;
        // 用户线程调用lws_cancel_service(). 进入LWS_CALLBACK_EVENT_WAIT_CANCELLED事件然后遍历所有
        // websocket,生成可写事件
        /*
         * When the "spam" threads add a message to the ringbuffer,
         * they create this event in the lws service thread context
         * using lws_cancel_service().
         *
         * We respond by scheduling a writable callback for all
         * connected clients.
         */
        lws_start_foreach_llp(struct per_session_data__minimal **,
                              ppss, vhd->pss_list)
        {
            lws_callback_on_writable((*ppss)->wsi);
        }
        lws_end_foreach_llp(ppss, pss_list);
        break;
    }

    // 回调函数最终要返回0，否则无法创建服务器
    return 0;
}

/**
 * 支持的WebSocket子协议数组
 * 子协议即JavaScript客户端WebSocket(url, protocols)第2参数数组的元素
 * 你需要为每种协议提供回调函数
 */
struct lws_protocols protocols[] = {
    {//协议名称，协议回调，接收缓冲区大小
        "ws",
        protocol_ws_callback,
        sizeof(struct per_session_data__minimal),
        MAX_PAYLOAD_SIZE,
        0,
        NULL,
        0
     },
    {
        NULL, NULL, 0 // 最后一个元素固定为此格式
    }
};


int main(int argc, char **argv)
{
    // 信号处理函数
    signal(SIGTERM, sighdl);

    struct lws_context_creation_info ctx_info = {0};
    ctx_info.port = 8000;
    ctx_info.iface = NULL; // 在所有网络接口上监听
    ctx_info.protocols = protocols;
    ctx_info.gid = -1;
    ctx_info.uid = -1;
    ctx_info.options = LWS_SERVER_OPTION_VALIDATE_UTF8;

    struct lws_context *context = lws_create_context(&ctx_info);

    while (!exit_sig)
    {
        lws_service(context, 1000);
    }

    lws_context_destroy(context);

    return 0;
}
