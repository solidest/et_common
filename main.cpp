
#include <assert.h>
#include <iostream>
#include <string.h>

#include "RKLOG/server/rklog_server.h"
#include "RKLOG/client/rklog_client.h"
#include "RKDB/server/rkdb_server.h"

using namespace std;

int start_logclient()
{
    rkLog rlog("192.168.136.138", 5000);
    for(int i=0; i<1000; i++)
    {
        rlog.log("test");        
    }
}

int test_logserver()
{
    rklog_serv_start("192.168.136.138", 5000, "rklog");  
}

// int test_dbserver()
// {
//     //test connect
//     // start_dbserver("192.168136.138", 8000, 10);
//     // rkcpc_open(10);
//     // const char * msg2 = "test message";
//     // for(int i=0; i<10; i++)
//     // {
//     //     int conn = rkcpc_getid("192.168136.138", 8000);
//     //     int ret = rkcpc_call(conn, 0, msg2, 7);
//     //     printf("result = %d\n", ret);
//     //     char buf[7];
//     //     ret = rkcpc_pcall(conn, 0, msg2, 7, buf, 7);
//     //     assert(ret == 7);
//     //     printf("result = %s\n", buf);
//     // }
// }

int test_dbserver()
{
    RkdbServer db(254);
    string pjname = "test";
    string pjnotes = "test_notes";
    string pjcontent = "content of project test";
    auto pid = db.NewProject(pjname, pjnotes);
    db.SaveProject(pid, pjcontent);

    string pjc = db.OpenProject(pid);
    assert(pjcontent == pjc);


    db.DeleProject(pid);

    

    //auto n = db.GetNow();
    //for(int i=0; i<4099; i++)
        //std::cout << db.GetNewId() << endl;
}

int main(int argc, const char * argv[]) {
    
    int ret = 0;
    cout<<endl<<"<<<<<<<<<<<<<<<<<<<  begin test  >>>>>>>>>>>>>>>>>>>"<<endl<<endl;  
    test_dbserver();
    //start_logclient();  
    //ret = test_logserver();

    //start_logclient();
    //ret = test_rkcp_client();
    //ret = test_rkcp_server();
    //ret = test_dbserver();
    cout<<endl<<"<<<<<<<<<<<<<<<<<<<  end   test  >>>>>>>>>>>>>>>>>>>"<<endl<<endl;
    return ret;
}
