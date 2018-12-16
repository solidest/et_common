
var count = 0;
var out = function showProjectInfo(itype, source, info)
{
    console.log(source + ":" + itype + "> " + info);
    count += 1;
    if(count>7)
    addon.StopLogServer();
}

var addon = require('./rklog_server');
addon.StartLogServer("127.0.0.1", 5000, out);
