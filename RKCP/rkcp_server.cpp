
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <ctime> 
#include "rkcp_server.h"

typedef struct LIST_ITEM
{
    rkcpconn conn;
    struct LIST_ITEM * prev, * next;
    long long out_time;
    bool isBusy;
} POOL_LIST_ITEM;

typedef struct LIST_HEADER 
{
    struct LIST_ITEM * first;
    struct LIST_ITEM * last;
} POOL_LIST_HEADER;


static int pool_size = 0;
static POOL_LIST_HEADER pool_header;
static POOL_LIST_ITEM *pool = NULL;

//initial client connection pool
//connPoolSize : size of connection pool
void rkcps_start(int connPoolSize,int (*output)(const char *buf, int len, struct IKCPCB *kcp, void *user))
{
    pool_size = connPoolSize;
    pool = (LIST_ITEM*)malloc(sizeof(struct LIST_ITEM)*pool_size);
    pool_header.first = pool;
    pool_header.last = &pool[pool_size-1];
    for(int i=0; i<pool_size; i++)
    {
        pool[i].next = (i==pool_size-1) ? pool : &pool[i+1];
        pool[i].prev = (i==0) ? &pool[pool_size-1] : &pool[i-1];
    }

    for(int i=0; i<pool_size; i++)
    {
        pool[i].isBusy = false;
        pool[i].out_time = 0;
        rkcpconn& conn = pool[i].conn;
        conn.call_id = 0;
        conn.callmode = 0;
        conn.data_len = 0;
        conn.recved_len = 0;
        conn.conn_kcp = ikcp_create(0, 0);
        conn.conn_kcp->output = output;
    }
}

//release all connections in pool
void rkcps_stop()
{
    for(int i=0; i<pool_size; i++)
    {
        ikcp_release(pool[i].conn.conn_kcp);
    }
    free(pool);
}

//get the connection of client
//input ip & port of client
//return connection id
rkcpconn * rkcps_open_conn(int clientip, int clientport, long long now_time)
{
    int conv = MAKE_CONV(clientip, clientport);

    //find conn in alive
    auto it = pool_header.first;
    while(it->isBusy)
    {
        if(it->out_time<now_time)
        {
            auto bkit = it;
            it = it->next;
            rkcps_close_conn(&bkit->conn);
            continue;
        }
        else if(it->conn.conn_kcp->conv == conv)
        {
            return &it->conn;
        }
        else
        {
            it = it->next;
            if(it == pool_header.first) return NULL;
        }
    }

    //create a new
    it = pool_header.last;
    if(it->isBusy) return NULL;
    it->isBusy = true;
    it->out_time = now_time + RKCP_TIMEOUT_COUNT;
    it->conn.conn_kcp->conv = conv;
    pool_header.first = it;
    pool_header.last = it->prev;
    return &it->conn;
}

//close the connection of client
void rkcps_close_conn(rkcpconn* rkcp)
{
    if(rkcp == NULL) return;
    rkcp->call_id = 0;
    rkcp->callmode = 0;
    rkcp->data_len = 0;
    rkcp->recved_len = 0;
    reset_kcp(rkcp->conn_kcp);
    rkcp->conn_kcp->conv = 0;
    if(rkcp->conn_kcp->user) 
    {
        free(rkcp->conn_kcp->user);
        rkcp->conn_kcp->user = NULL;
    }

    POOL_LIST_ITEM * it = (POOL_LIST_ITEM*)rkcp;
    it->isBusy = false;
    it->out_time = 0;
    if(it == pool_header.first)
    {
        pool_header.first = it->next;
        pool_header.last = it;
        return;
    }
    if(it == pool_header.last)
    {
        return;
    }
    //delete
    it->prev->next = it->next;
    it->next->prev = it->prev;
    //insert
    it->next = pool_header.first;
    it->prev = pool_header.last;
    pool_header.first->prev = it;
    pool_header.last->next = it;
    pool_header.last = it;
    return;
}



//update all alive kcp
void rkcps_update(unsigned long long now_time)
{
    auto it = pool_header.first;
    while(it->isBusy)
    {
        if(it->out_time<now_time)
        {
            auto bkit = it;
            it = it->next;
            rkcps_close_conn(&bkit->conn);
            continue;
        }
        else
        {
            ikcp_update(it->conn.conn_kcp, now_time);
            it = it->next;
            if(it == pool_header.first) return;
        }
    }
}