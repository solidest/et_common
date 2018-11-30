
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctime> 
#include <iostream>

#include "rkcp_client.h"

using namespace std;

struct RKCPCONN
{
    ikcpcb * conn_kcp;
    int conn_socket;
    int conn_serverip;
    int conn_serverport;
};

typedef struct RKCPCONN rkcpconn;


static int conn_pool_size = 0;
static rkcpconn* p_conn_pool = NULL;

//callback for kcp
static int udp_output(const char *buf, int len, ikcpcb *kcp, void *user)
{
    rkcpconn& conn = *(rkcpconn*)user;
    send(conn.conn_socket, buf, len, 0);
    printf("send udp(%d)\n", len);
}


//initial client connection pool
 void rkcpc_open(int connPoolSize)
{
    conn_pool_size = connPoolSize;
    p_conn_pool = (rkcpconn *)malloc(sizeof(rkcpconn)*connPoolSize);
    memset(p_conn_pool, 0, sizeof(rkcpconn)*connPoolSize);
}

//release all connections in pool
 void rkcpc_close()
{
    for(int i=0; i<conn_pool_size; i++)
    {
        if(p_conn_pool[i].conn_kcp!=NULL)
        {
            ikcp_release(p_conn_pool[i].conn_kcp);
            close(p_conn_pool[i].conn_socket);
        }
    }
    free(p_conn_pool);
}

//get the connection id of server
//input ip & port of server
//return connection id
int rkcpc_getid(const char* serverip, int serverport)
{
    int iserverip = inet_addr(serverip);
    int idx = 0;
    while(idx<conn_pool_size && p_conn_pool[idx].conn_kcp!=NULL)
    {
        if(p_conn_pool[idx].conn_serverip == iserverip && p_conn_pool[idx].conn_serverport == serverport)
            return idx;
        idx++;
    }
    if(idx==conn_pool_size) return RKCP_ERR_POOLISFULL;

    struct sockaddr_in ser_addr;
    memset(&ser_addr, 0, sizeof(ser_addr));
    ser_addr.sin_addr.s_addr = iserverip;
    ser_addr.sin_port = htons(serverport);
    ser_addr.sin_family = AF_INET;
    int isock = socket(PF_INET,SOCK_DGRAM , 0);
    if(isock<0) return RKCP_ERR_SOCKETERROR;
    struct timeval tv;
	tv.tv_sec = 0; 
	tv.tv_usec = RKCP_TIMEOUT_COUNT*1000;
	setsockopt(isock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    if (connect(isock, (struct sockaddr *)&ser_addr, sizeof(struct sockaddr_in)) == -1)
        return RKCP_ERR_SOCKETERROR;

    struct sockaddr_in loc_addr;
    socklen_t len = sizeof(loc_addr);
	memset(&loc_addr, 0, len);
	if (-1 == getsockname(isock, (struct sockaddr *)&loc_addr, &len)) {
		close(isock);
        return RKCP_ERR_SOCKETERROR;
	}

    if(loc_addr.sin_addr.s_addr <<16 != ser_addr.sin_addr.s_addr<<16)
        return RKCP_ERR_CROSSDOMAIN;

    ikcpcb *kcp = ikcp_create(MAKE_CONV(loc_addr.sin_addr.s_addr, loc_addr.sin_port), &p_conn_pool[idx]);
    ikcp_nodelay(kcp, 1, 10, 2, 1);
    kcp->output = udp_output;


    p_conn_pool[idx].conn_serverip = ser_addr.sin_addr.s_addr;
    p_conn_pool[idx].conn_serverport = serverport;
    p_conn_pool[idx].conn_kcp = kcp;
    p_conn_pool[idx].conn_socket = isock;
    return idx;   
}


//call a remote server
//connid : connection id of server
//buff & len : the data for send to server
//retrun status code(>0) or errorcode(<0)
int rkcpc_call(int connid, int callid, const char * send_buff, int send_len)
{

    ikcpcb * conn = p_conn_pool[connid].conn_kcp;
    printf("conv := %d \n", conn->conv);
    unsigned long long header = MAKE_CALL_HEADER(RKCP_CALLMODE_VOID, callid, send_len);
    char recv_buff[2048] = {0};
    if(ikcp_send(conn, (char *)&header, 8)!=0 || ikcp_send(conn, send_buff, send_len)!=0)
        return RKCP_ERR_KCP;
    ikcp_update(conn, get_now());

    int to_count = 0;
    int ret = 0;

    while(true)
    {

        auto reci = recvfrom(p_conn_pool[connid].conn_socket, recv_buff, 2048, 0, NULL, NULL);
        
        if(reci == 0)
        {
            printf("recv 0\n");
        }

        if(reci < 0)
        {
            printf("time out\n");
            to_count += 1;
            if (to_count>10)
                return RKCP_ERR_TIMEOUT;
        }

        if (reci > 0) 
        {
            ikcp_input(conn, recv_buff, reci);
            printf("recv upd := size(%ld)\n", reci);           
        }

        if (reci >0 || ikcp_waitsnd(conn)>0)
            ikcp_update(conn, get_now());

        printf("peeksize := (%d)\n", ikcp_peeksize(conn)); 
        printf("waitsend1 %d\n", ikcp_waitsnd(conn));  
        if(ikcp_peeksize(conn)>=8)
        {
            header = 0;
            if(ikcp_recv(conn, (char*)&header, 8)!=8)
            {
                reset_kcp(conn);
                return RKCP_ERR_KCP;
            }

            printf("recv header := %lld\n", header);
            ret = GET_RESULT(header);
        }
        
        if(ret != 0)
        {
            if(ikcp_waitsnd(conn) == 0 )
            {
                printf("finish!\n");
                reset_kcp(conn);
                return ret; 
            }
            else
            {
                printf("waitsnd2 %d\n", ikcp_waitsnd(conn));
            }
     
        }

    }

}

//call a remote server and get result data
//connid : connection id of server
//send_buff & send_len : the data for send to server
//recv_buff & recv_len : the buff for recv result
//retrun the len have recved or errorcode (<0)
int rkcpc_pcall(int connid, int callid, const char * send_buff, int send_len, char * recv_buff, int recv_len)
{
    rkcpconn & conn = p_conn_pool[connid];
    unsigned long long header = MAKE_CALL_HEADER(RKCP_CALLMODE_DATA, callid, send_len);
    char buf[2048] = {0};
    bool is_recv_header = false;
    int data_len = 0;
    if(ikcp_send(conn.conn_kcp, (char *)&header, 8)!=0 || ikcp_send(conn.conn_kcp, send_buff, send_len)!=0)
        return RKCP_ERR_KCP;
    while(true)
    {
        ikcp_update(conn.conn_kcp, get_now());
        auto reci = recvfrom(conn.conn_socket, recv_buff, sizeof(recv_buff), 0, NULL, NULL);
        if(reci < 0)
        {
            reset_kcp(conn.conn_kcp);
            return RKCP_ERR_TIMEOUT;
        }

        if (reci > 0) ikcp_input(conn.conn_kcp, recv_buff, reci);
        if(!is_recv_header && ikcp_peeksize(conn.conn_kcp)>=8)
        {
            header = 0;
            if(ikcp_recv(conn.conn_kcp, (char*)&header, 8)!=8)
            {
                reset_kcp(conn.conn_kcp);
                return RKCP_ERR_KCP;
            }
            if(!CHECK_HEADER(header))
                return RKCP_ERR_KCP;
            data_len = GET_DATALEN(header);
            is_recv_header = true;
            if(data_len<=0)
            {
                reset_kcp(conn.conn_kcp);
                return RKCP_ERR_UNKNOWN;
            }
        }
        if(is_recv_header && ikcp_peeksize(conn.conn_kcp)>=data_len)
        {
            int ret = ikcp_recv(conn.conn_kcp, recv_buff, recv_len);
            reset_kcp(conn.conn_kcp);
            return ret;
        }
    }
}