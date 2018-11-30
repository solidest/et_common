/*
** rkcp.h for RKCP in /home/byx/et/et_common/RKCP
**
** Made by header_template
** Login   <>
**
** Started on  Tue Nov 27 7:24:06 PM 2018 header_template
** Last update Thu Nov 28 4:39:23 PM 2018 header_template
*/

#ifndef RKCP_H_
# define RKCP_H_

#define GET_RESULT(header) (((header) << 11) >> 59)
#define GET_DATALEN(header) (0x00000000FFFFFFFF & header)
#define GET_CALLMODE(header) GET_RESULT(header)
#define GET_CALLID(header) (((header) << 16) >> 48)
#define CHECK_HEADER(header) (header>>53==1210)
#define MAKE_CALL_HEADER(callmode, callid, len) ((((unsigned long long)1210)<<53) | ((((unsigned long long)callmode)<<59)>>11) \
                | (((unsigned long long)(unsigned short)callid)<<32) | ((unsigned long long)(unsigned int)len))
#define MAKE_RESP_HEADER(result, percent, len) MAKE_CALL_HEADER(result, percent, len)
				
#define MAKE_CONV(clientip, clientport) (((clientip) & 0xFFFF0000) | ((clientport) & 0x0000FFFF))


#define RKCP_CALLMODE_VOID 1
#define RKCP_CALLMODE_DATA 2
#define RKCP_CALLMODE_PUSH 3
#define RKCP_CALLMODE_PULL 4

#define RKCP_RESULT_OK 1

#define RKCP_TIMEOUT_COUNT 100   //milliseconds

#define RKCP_ERR_TIMEOUT -1
#define RKCP_ERR_KCP -2
#define RKCP_ERR_SOCKETERROR -3
#define RKCP_ERR_POOLISFULL -4
#define RKCP_ERR_CROSSDOMAIN -5

#define RKCP_ERR_UNKNOWN -100

#include <chrono>
#include "ikcp.h"
using namespace std;

//reset kcp object
inline void reset_kcp(ikcpcb* kcp)
{
    const IUINT32 IKCP_RTO_MIN = 100;		// normal min rto
    const IUINT32 IKCP_RTO_DEF = 200;
    const IUINT32 IKCP_WND_SND = 32;
    const IUINT32 IKCP_WND_RCV = 128;       // must >= max fragment size
    const IUINT32 IKCP_MTU_DEF = 1400;
    const IUINT32 IKCP_INTERVAL	= 100;
    const IUINT32 IKCP_OVERHEAD = 24;
    const IUINT32 IKCP_DEADLINK = 20;
    const IUINT32 IKCP_THRESH_INIT = 2;


    IKCPSEG *seg;
    while (!iqueue_is_empty(&kcp->snd_buf)) {
			seg = iqueue_entry(kcp->snd_buf.next, IKCPSEG, node);
			iqueue_del(&seg->node);
			free(seg);
    }
    while (!iqueue_is_empty(&kcp->rcv_buf)) {
        seg = iqueue_entry(kcp->rcv_buf.next, IKCPSEG, node);
        iqueue_del(&seg->node);
        free(seg);
    }
    while (!iqueue_is_empty(&kcp->snd_queue)) {
        seg = iqueue_entry(kcp->snd_queue.next, IKCPSEG, node);
        iqueue_del(&seg->node);
        free(seg);
    }
    while (!iqueue_is_empty(&kcp->rcv_queue)) {
        seg = iqueue_entry(kcp->rcv_queue.next, IKCPSEG, node);
        iqueue_del(&seg->node);
        free(seg);
    }

	kcp->snd_una = 0;
	kcp->snd_nxt = 0;
	kcp->rcv_nxt = 0;
	kcp->ts_recent = 0;
	kcp->ts_lastack = 0;
	kcp->ts_probe = 0;
	kcp->probe_wait = 0;
	kcp->snd_wnd = IKCP_WND_SND;
	kcp->rcv_wnd = IKCP_WND_RCV;
	kcp->rmt_wnd = IKCP_WND_RCV;
	kcp->cwnd = 0;
	kcp->incr = 0;
	kcp->probe = 0;
	kcp->mtu = IKCP_MTU_DEF;
	kcp->mss = kcp->mtu - IKCP_OVERHEAD;
	kcp->stream = 0;

	iqueue_init(&kcp->snd_queue);
	iqueue_init(&kcp->rcv_queue);
	iqueue_init(&kcp->snd_buf);
	iqueue_init(&kcp->rcv_buf);
	kcp->nrcv_buf = 0;
	kcp->nsnd_buf = 0;
	kcp->nrcv_que = 0;
	kcp->nsnd_que = 0;
	kcp->state = 0;
	//kcp->acklist = NULL;
	//kcp->ackblock = 0;
	kcp->ackcount = 0;
	kcp->rx_srtt = 0;
	kcp->rx_rttval = 0;
	kcp->rx_rto = IKCP_RTO_DEF;
	kcp->rx_minrto = IKCP_RTO_MIN;
	kcp->current = 0;
	kcp->interval = IKCP_INTERVAL;
	kcp->ts_flush = IKCP_INTERVAL;
	kcp->nodelay = 0;
	kcp->updated = 0;
	kcp->logmask = 0;
	kcp->ssthresh = IKCP_THRESH_INIT;
	kcp->fastresend = 0;
	kcp->nocwnd = 0;
	kcp->xmit = 0;
	kcp->dead_link = IKCP_DEADLINK;
	//kcp->output = NULL;
	//kcp->writelog = NULL;
}

  
//get now clock for milliseconds
inline int64_t get_now()
{
    return chrono::time_point_cast<chrono::milliseconds>(chrono::steady_clock::now()).time_since_epoch().count(); 
}

#endif /* !RKCP_H_ */
