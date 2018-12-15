
var count = 0;
var out = function showProjectInfo(itype, info)
{
    console.log(itype + "> " + info);
    count += 1;
    if(count>3)
    addon.StopLogServer();
}

var addon = require('./rklog_server');
addon.StartLogServer("127.0.0.1", 5000, out);
