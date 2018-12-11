
#include <assert.h>
#include <iostream>
#include <string.h>

#include "RKLOG/server/rklog_server.h"
#include "RKLOG/client/rklog_client.h"
#include "RKDB/server/rkdb_server.h"

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"


using namespace std;
using namespace rapidjson;

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
    string lp = db.GetProjectInfoList();

    Document d;
    d.Parse(lp.c_str());
    int ic1 = d.Size();
    cout<<"pass1: "<<lp<<endl;

    //new project info
    Document doc(kObjectType);
    doc.AddMember("name", "project name test", doc.GetAllocator());
    doc.AddMember("notes", "notes of project", doc.GetAllocator());
    Value v(kArrayType);
    v.PushBack("test", doc.GetAllocator());
    v.PushBack("demo", doc.GetAllocator());
    doc.AddMember("tag", v, doc.GetAllocator());

    StringBuffer buffer;
    Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    string strJson = buffer.GetString();
    auto id = db.NewProjectInfo(strJson);
   
    //test save content
    string pjcontent("content of project");
    db.SaveProject(id, pjcontent);
    string pjc = db.OpenProject(id);
    assert(pjcontent == pjc);
    string newc = "new content";
    db.SaveProject(id, newc);
    pjc = db.OpenProject(id);
    assert(pjcontent != pjc);
    cout<<"pass2: "<<pjc<<endl;

    //test update info
    string ninfo = "new info";
    db.UpdateProjectInfo(id, ninfo);

    lp = db.GetProjectInfoList();
    d.Parse(lp.c_str());
    int ic2 = d.Size();
    assert((ic2-1)==ic1);
    assert(lp.find(ninfo)>0);
    cout<<"pass3: "<<lp<<endl;
    db.DeleProject(id);
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
