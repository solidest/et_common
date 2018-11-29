/*
** rkcp_server.h for RKCP in /home/byx/et/et_common/RKCP
**
** Made by header_template
** Login   <>
**
** Started on  Tue Nov 27 10:57:01 PM 2018 header_template
** Last update Tue Nov 27 10:57:01 PM 2018 header_template
*/

#ifndef RKCP_SERVER_H_
# define RKCP_SERVER_H_

#include "rkcp.h"
#include "ikcp.h"

#ifdef __cplusplus
extern "C" {
#endif


struct RKCPCONN
{
    ikcpcb * conn_kcp;
    int data_len;
    int recved_len;
    int call_id;
    int callmode;
};


typedef struct RKCPCONN rkcpconn;


//initial client connection pool
//connPoolSize : size of connection pool
void rkcps_start(int connPoolSize, int (*output)(const char *buf, int len, struct IKCPCB *kcp, void *user));

//release all connections in pool
void rkcps_stop();
 
//get the connection of client
//input ip & port of client
//return connection id
rkcpconn * rkcps_open_conn(int clientip, int clientport, long long now_time);

//close the connection of client
void rkcps_close_conn(rkcpconn * rkcp);

//update all alive kcp
void rkcps_update(unsigned long long now_time);


#ifdef __cplusplus
}
#endif


#endif /* !RKCP_SERVER_H_ */
