
#include <assert.h>
#include <iostream>
#include <string.h>

#include "RKCP//rkcp_client.h"
#include "RKCP//rkcp_server.h"
using namespace std;
 

int main(int argc, const char * argv[]) {
    cout<<endl<<"<<<<<<<<<<<<<<<<<<<begin test>>>>>>>>>>>>>>>>>>>"<<endl<<endl;

    //test client
    rkcpc_open(2);
    int i = rkcpc_getid("127.0.0.1", 8000);
    assert(i==0);
    i = rkcpc_getid("192.168.136.1", 60001);
    assert(i==1);
    i = rkcpc_getid("127.0.0.1", 8001);
    assert(i<0);
    i = rkcpc_getid("192.168.136.1", 60001);
    assert(i==1);
    cout<<"pass1"<<endl;

    const char* msg = "test message";
    int res = rkcpc_call(i, 0, msg, strlen(msg));
    assert(res == RKCP_ERR_TIMEOUT);
    cout<<"pass2"<<endl;

    //test server
    int size = 20;
    rkcps_start(size);
    auto conn = rkcps_open_conn(22222, 22222, get_now());
    assert(conn != nullptr);
    conn = rkcps_open_conn(11111, 11111, get_now());
    assert(conn != nullptr);

    while(true)
    {
        conn = rkcps_open_conn(rand(), rand(), get_now());
        if(conn == nullptr) 
            break;
    }
    conn = rkcps_open_conn(22222, 22222, get_now());
    assert(conn != nullptr);
    conn = rkcps_open_conn(11111, 11111, get_now());
    assert(conn != nullptr);
    rkcps_close_conn(conn);
    conn = rkcps_open_conn(33333, 33333, get_now());
    assert(conn != nullptr);
    conn = rkcps_open_conn(44444, 44444, get_now());
    assert(conn == nullptr);

    rkcps_stop();
    cout<<"pass3"<<endl;


    //test connect

    //start_udpserver();
    // res = rkcpc_call(0, i,  msg, strlen(msg));
    // assert(res == 1);


    rkcpc_close();
    cout<<endl<<"<<<<<<<<<<<<<<<<<<<end   test>>>>>>>>>>>>>>>>>>>"<<endl<<endl;
    return 0;
}
