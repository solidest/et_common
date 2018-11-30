
#include <string.h>
#include "rklog_server.h"
#include "..//..//RKCP//rkcp_server.h"
#include "rkuv.h"

static uv_loop_t *loop;
static uv_udp_t recv_serv;
static uv_check_t update_kcp;

static void udp_sv_send_cb(uv_udp_send_t* req, int status)
{
	if (status != 0) {
		//TODO
        printf("error1\n");
	}

    if(req)
        free(req);

}

//callback for kcp
static int udp_output(const char *buf, int len, ikcpcb *kcp, void *user)
{
    if(user)
    {
        uv_udp_send_t* req = (uv_udp_send_t *)malloc(sizeof(uv_udp_send_t)+len);

        uv_buf_t re_buf = uv_buf_init((char *)&req[1], len);
        uv_udp_send(req, &recv_serv, &re_buf, len, (struct sockaddr *)user, udp_sv_send_cb);        
    }
    else
    {
        printf("error2\n");
        //TODO
    }
    
}


//read userdata in kcp package
static void on_read_kcpdata(rkcpconn& conn) 
{
    switch(conn.callmode)
    {
        case RKCP_CALLMODE_VOID:
            if(conn.recved_len == 0 && ikcp_peeksize(conn.conn_kcp)>=conn.data_len)
            {
                char * buf = (char *)malloc(conn.data_len);
                conn.recved_len = ikcp_recv(conn.conn_kcp, buf, conn.data_len);
                rklog(buf);
                unsigned long long header = MAKE_RESP_HEADER(RKCP_RESULT_OK, 0, conn.data_len);
                ikcp_send(conn.conn_kcp, (char *)&header, 8);
                free(buf);
            }
            if(conn.recved_len==conn.data_len && ikcp_waitsnd(conn.conn_kcp)==0)
                rkcps_close_conn(&conn);
            break;
    }


}

//read kcp header
static void on_read_kcpheader(rkcpconn& conn) 
{
    if(ikcp_peeksize(conn.conn_kcp)>=8)
    {
        unsigned long long header = 0;
        if(ikcp_recv(conn.conn_kcp, (char *)&header, 8)!=8)
        {
            printf("error3\n");
            //TODO
            return;
        }
        if(!CHECK_HEADER(header))
        {
            printf("error4\n");
            //TODO
            return;
        }
        conn.data_len = GET_DATALEN(header);
        printf("len:%d\n", conn.data_len);
        conn.call_id = GET_CALLID(header);
        printf("call_id:%d\n", conn.call_id);
        conn.callmode = GET_CALLMODE(header);
        printf("call_mode:%d\n", conn.callmode);
        if(ikcp_peeksize(conn.conn_kcp)>0)
            on_read_kcpdata(conn);
    }
}

//dispatch udp package
static void on_read(uv_udp_t *req, ssize_t nread, const uv_buf_t *buf, const struct sockaddr *addr, unsigned flags) 
{

    auto caddr = (const struct sockaddr_in*) addr;
    auto now_time = uv_now(loop);
    if(caddr == nullptr)
    {
        assert(nread==0);
        return;
    }

    auto conn = rkcps_open_conn(caddr->sin_addr.s_addr, caddr->sin_port, now_time);
    if(conn == NULL)
    {
        rklog("pool is full");
        return;
    }
    if(conn->conn_kcp->user == NULL)
    {
        conn->conn_kcp->user = malloc(sizeof(struct sockaddr_in));
        memcpy(conn->conn_kcp->user, addr, sizeof(struct sockaddr_in));
    } 

    if(nread>0) ikcp_input(conn->conn_kcp, buf->base, nread);
    conn->callmode == 0 ? on_read_kcpheader(*conn) : on_read_kcpdata(*conn);

}

//malloc buffer for uvlib
static void uv_alloc_buf(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
    if(handle->data) {
        free(handle->data);
        handle->data = NULL;
    }

    handle->data = malloc(suggested_size);
    buf->base = (char *)handle->data;
    buf->len = suggested_size;
}

//update kcp
static void on_update(uv_check_t* handle)
{
    rkcps_update(uv_now(loop));
    rklog("update rkcps");
}

//start udp servver
int rklog_start(const char* serverip, int serverport, int connpoolsize)
{
    rkcps_start(connpoolsize, udp_output);

    loop = uv_default_loop();

    uv_udp_init(loop, &recv_serv);
    struct sockaddr_in recv_addr;
    uv_ip4_addr(serverip, serverport, &recv_addr);
    uv_udp_bind(&recv_serv, (const struct sockaddr *)&recv_addr, UV_UDP_REUSEADDR);
    uv_udp_recv_start(&recv_serv, uv_alloc_buf, on_read);

    uv_check_init(loop, &update_kcp);
    uv_check_start(&update_kcp, on_update);

    rklog("start rklog server on %s:%d", serverip, serverport);

    return uv_run(loop, UV_RUN_DEFAULT);
}
