/*------------------------------------------------------------------------------
 * test_tcpclient.c
 *
 * author  : sujinglan
 * version : $Revision: 1.1 $ $Date: 2008/07/17 21:48:06 $
 * history : 2022/11/17 1.0  new
 *-----------------------------------------------------------------------------*/
#include "cors.h"

static void on_rsp_cb(uv_write_t* req, int status)
{
    free(req);
}

static void on_timer_cb(uv_timer_t* handle)
{
    uv_buf_t buf;
    uv_write_t *wr;
    int ret;
    static int id=1,cnt;
    static char buff[1024];

    if (!uv_is_writable(handle->data)) return;
    cnt++;
    if (cnt>10 ) id=2;
    if (cnt>60 ) id=1;
    if (cnt>80 ) id=2;
    if (cnt>90 ) id=1;
    if (cnt>100) id=3;
    if (cnt>120) id=2;
    if (cnt>150) id=1;

    char tmp[256];
    sprintf(tmp,"MONITOR-SOURCE cors A00%d",id);

    buf.base=buff;
    strcpy(buf.base,tmp);
    buf.len=strlen(buf.base);

    wr=malloc(sizeof(uv_write_t));

    if ((ret=uv_write(wr,handle->data,&buf,1,on_rsp_cb))!=0) {
        log_trace(1,"failed to send req: %s\n",
                uv_strerror(ret));
        free(wr);
        return;
    }
}

static void on_timer_bsta_distr_cb(uv_timer_t* handle)
{
    uv_buf_t buf;
    uv_write_t *wr;
    static char buff[1024];
    int ret;

    if (!uv_is_writable(handle->data)) return;

    buf.base=buff;
    strcpy(buf.base,"MONITOR-BSTADISTR cors FuJian physics-station");
    buf.len=strlen(buf.base);

    wr=malloc(sizeof(uv_write_t));

    if ((ret=uv_write(wr,handle->data,&buf,1,on_rsp_cb))!=0) {
        log_trace(1,"failed to send req: %s\n",
                uv_strerror(ret));
        free(wr);
        return;
    }
}

static void alloc_callback(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
    buf->base=malloc(suggested_size);
    buf->len=suggested_size;
}

static void on_read_cb(uv_stream_t *str, ssize_t nread, const uv_buf_t *buf)
{
    if (nread<0) {
        uv_close((uv_handle_t*)str,NULL);
        free(buf->base);
        return;
    }
    buf->base[nread]='\0';
    fprintf(stderr,"%s",buf->base);
    free(buf->base);
}

static void connect_cb(uv_connect_t *req, int status)
{
    if (status<0) {
        fprintf(stderr,"connect error: %s\n",uv_strerror(status));
        return;
    }
    uv_read_start((uv_stream_t*)req->data,alloc_callback,on_read_cb);
}

int main(int argc, const char *argv[])
{
    log_trace_open("");
    log_set_level(1);

    uv_connect_t connection_request;
    uv_timer_t timer_write_req;
    uv_timer_t timer_bsta_distr;
    uv_loop_t loop;
    uv_tcp_t tcp_client;

    memset(&loop,0,sizeof(uv_loop_t));
    memset(&tcp_client,0,sizeof(uv_tcp_t));
    memset(&connection_request,0,sizeof(uv_connect_t));
    struct sockaddr_in sa;

    uv_ip4_addr("127.0.0.1",7999,&sa);
    uv_loop_init(&loop);
    uv_tcp_init(&loop,&tcp_client);

    timer_write_req.data=&tcp_client;
    uv_timer_init(&loop,&timer_write_req);
    uv_timer_start(&timer_write_req,on_timer_cb,0,1000);

    timer_bsta_distr.data=&tcp_client;
    uv_timer_init(&loop,&timer_bsta_distr);
    uv_timer_start(&timer_bsta_distr,on_timer_bsta_distr_cb,0,2000);

    connection_request.data=&tcp_client;
    uv_tcp_connect(&connection_request,&tcp_client,(const struct sockaddr *)&sa,
            connect_cb);
    uv_run(&loop,UV_RUN_DEFAULT);
    return 0;
}