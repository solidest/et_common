
#include <assert.h>
#include <iostream>
#include <string.h>

#include "RKLOG/server/rklog_server.h"
#include "RKLOG/client/rklog_client.h"
#include "RKDB/server/rkdb_server.h"

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "rpc/client.h"
#include "rpc/rpc_error.h"


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


int test_dbserver()
{
    RkdbServer db(254, NULL);
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

void* _start_dbserver(void* args)
{
    RkdbServer db(1024, NULL);
    rkdb_serv_start(db, "192.168.136.138", 8000);
}

int test_start_dbserver()
{
    //参数依次是：创建的线程id，线程参数，调用的函数，传入的函数参数
    pthread_t pth;
    int ret = pthread_create(&pth, NULL, _start_dbserver, NULL);

    sleep(1);
    rpc::client c("192.168.136.138", 8000);

    try 
    {
        // default timeout is 5000 milliseconds
        const uint64_t short_timeout = 5000;
        c.set_timeout(short_timeout);
        auto pl = c.call("GetProjectInfoList").as<string>();
        cout<<"pass1: "<<pl<<endl;
    } 
    catch (rpc::timeout &t) 
    {
        // will display a message like
        // rpc::timeout: Timeout of 50ms while calling RPC function 'sleep'
        std::cout << t.what() << std::endl;
    }
    catch (rpc::rpc_error &t)
    {
        std::cout << t.what() << std::endl;
    }

    pthread_exit(NULL);
    return 0;
}

int main(int argc, const char * argv[]) {
    
    int ret = 0;
    cout<<endl<<"<<<<<<<<<<<<<<<<<<<  begin test  >>>>>>>>>>>>>>>>>>>"<<endl<<endl;
    test_start_dbserver();

    //test_dbserver();
    //start_logclient();  
    //ret = test_logserver();

    //start_logclient();
    //ret = test_rkcp_client();
    //ret = test_rkcp_server();
    //ret = test_dbserver();
    cout<<endl<<"<<<<<<<<<<<<<<<<<<<  end   test  >>>>>>>>>>>>>>>>>>>"<<endl<<endl;
    return ret;
}
