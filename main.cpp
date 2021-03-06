
#include <assert.h>
#include <iostream>
#include <string.h>

//#include "RKLOG/server/rklog_server.h"
#include "RKLOG/client/rklog_client.h"
#include "RKDB/server/rkdb_server.h"

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "rpc/client.h"
#include "rpc/rpc_error.h"


using namespace std;
using namespace rapidjson;


int test_logclient()
{
    RklogClient c("tester", "127.0.0.1", 5000);
    c.LogError("test error");
    c.LogDebug("test debug");
    c.LogWarning("test warning");
    c.LogInfo("test info");
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
    db.SetProject(id, pjcontent);
    string pjc = db.GetProject(id);
    assert(pjcontent == pjc);
    string newc = "new content";
    db.SetProject(id, newc);
    pjc = db.GetProject(id);
    assert(pjcontent != pjc);
    cout<<"pass2: "<<pjc<<endl;

    //test update info
    string ninfo = "new info";
    db.SetProjectInfo(id, ninfo);

    lp = db.GetProjectInfoList();
    d.Parse(lp.c_str());
    int ic2 = d.Size();
    assert((ic2-1)==ic1);
    assert(lp.find(ninfo)>0);
    cout<<"pass3: "<<lp<<endl;
    db.DelProject(id);
}


int test_start_dbserver()
{
    //参数依次是：创建的线程id，线程参数，调用的函数，传入的函数参数
    // pthread_t pth;
    // int ret = pthread_create(&pth, NULL, _start_dbserver, NULL);

    RkdbServer db(1024, "/home/solidest/et/db_data");
    rkdb_serv_start(db, "127.0.0.1", 8000);

    usleep(100000);
    rpc::client c("127.0.0.1", 8000);

    try 
    {
        // default timeout is 5000 milliseconds
        const uint64_t short_timeout = 5000;
        c.set_timeout(short_timeout);
        auto pl = c.call("GetProjectInfoList").as<string>();
        cout<<"pass1: "<<pl<<endl;

        auto id = c.call("NewProjectInfo", "test info").as<long long>();
        auto ept = c.call("GetProject", id).as<string>();
        assert(ept.size() == 0);
        pl = c.call("GetProjectInfoList").as<string>();
        cout<<"pass2: "<<pl<<endl;

        usleep(100000);
        string cont("content of project!!");
        c.call("SetProject", id, cont);
        string cont2 = c.call("GetProject", id).as<string>();
        assert(cont == cont2);
        c.call("SetProjectInfo", id, "test new info");
        pl = c.call("GetProjectInfoList").as<string>();
        cout<<"pass3: "<<pl<<endl;
        
        c.call("DelProject", id);
        pl = c.call("GetProjectInfoList").as<string>();
        cout<<"pass4: "<<pl<<endl;

    } 
    catch (rpc::timeout &t) 
    {
        // will display a message like
        // rpc::timeout: Timeout of 50ms while calling RPC function 'sleep'
        std::cout << t.what() << std::endl;
    }
    catch (rpc::rpc_error &t)
    {
        using err_t = std::tuple<int, std::string>;
        auto err = t.get_error().as<err_t>();
        std::cout << std::get<1>(err) << std::endl;
    }

    cin.ignore();

    rkdb_serv_stop();

    return 0;
}

int main(int argc, const char * argv[]) {
    
    int ret = 0;
    cout<<endl<<"<<<<<<<<<<<<<<<<<<<  begin test  >>>>>>>>>>>>>>>>>>>"<<endl<<endl;
    test_start_dbserver();
    //test_logserver();
    //test_logclient();
    cout<<endl<<"<<<<<<<<<<<<<<<<<<<  end   test  >>>>>>>>>>>>>>>>>>>"<<endl<<endl;
    return ret;
}
