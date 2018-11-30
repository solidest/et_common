
#include <assert.h>
#include <iostream>
#include <string.h>

#include "RKCP//rkcp_client.h"
#include "RKCP//rkcp_server.h"
#include "RKDB/server/rkdb_server.h"
#include "RKLOG/server/rklog_server.h"

using namespace std;

int start_logclient()
{
    rkcpc_open(2);
    for(int i=0; i<10; i++)
    {
        int h = rkcpc_getid("192.168.1.111", 5000);
        const char * msg = "test message";
        int res = rkcpc_call(h, 0, msg, strlen(msg)+1);
        printf("result := %d\n", res);
        printf("\n");
    }
    rkcpc_close();

}

int test_logserver()
{
    return rklog_start("192.168.1.111", 5000, 10);
    
    
}

int test_rkcp_client()
{
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

    const char * msg = "test message";
    int res = rkcpc_call(i, 0, msg, strlen(msg));
    assert(res == RKCP_ERR_TIMEOUT);
    rkcpc_close();
    cout<<"pass2"<<endl;
    return 0;
}

int test_rkcp_server()
{
    //test server
    int size = 20;
    rkcps_start(size, nullptr);
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

    return 0;
}
 

int test_dbserver()
{
    //test connect
    // start_dbserver("192.168136.138", 8000, 10);
    // rkcpc_open(10);
    // const char * msg2 = "test message";
    // for(int i=0; i<10; i++)
    // {
    //     int conn = rkcpc_getid("192.168136.138", 8000);
    //     int ret = rkcpc_call(conn, 0, msg2, 7);
    //     printf("result = %d\n", ret);
    //     char buf[7];
    //     ret = rkcpc_pcall(conn, 0, msg2, 7, buf, 7);
    //     assert(ret == 7);
    //     printf("result = %s\n", buf);
    // }
}

int main(int argc, const char * argv[]) {
    
    int ret = 0;
    cout<<endl<<"<<<<<<<<<<<<<<<<<<<  begin test  >>>>>>>>>>>>>>>>>>>"<<endl<<endl;  
    //start_logclient();  
    ret = test_logserver();
    //ret = test_rkcp_client();
    //ret = test_rkcp_server();
    //ret = test_dbserver();
    cout<<endl<<"<<<<<<<<<<<<<<<<<<<  end   test  >>>>>>>>>>>>>>>>>>>"<<endl<<endl;
    return ret;
}
