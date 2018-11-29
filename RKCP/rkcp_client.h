#ifndef __RKCP_CLIENT_H__
#define __RKCP_CLIENT_H__

#include "ikcp.h"
#include "rkcp.h"

#ifdef __cplusplus
extern "C" {
#endif

//initial client connection pool
//clientip : ip address of client
//connPoolSize : size of connection pool
void rkcpc_open(int connPoolSize);

//release all connections in pool
void rkcpc_close();

//get the connection id of server
//input ip & port of server
//return connection id
int rkcpc_getid(const char* serverip, int serverport);

//call a remote server
//connid : connection id of server
//callid : interface id of server
//buff & len : the data for send to server
//retrun status code(>0) or errorcode(<0)
int rkcpc_call(int connid, int callid, const char * send_buff, int send_len);

//call a remote server and get result data
//connid : connection id of server
//send_buff & send_len : the data for send to server
//recv_buff & recv_len : the buff for recv result
//retrun the len have recved or errorcode (<0)
int rkcpc_pcall(int connid, int callid, const char * send_buff, int send_len, char * recv_buff, int recv_len);

#ifdef __cplusplus
}
#endif

#endif /*__RKCP_CLIENT_H__*/