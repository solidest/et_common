

var out = function showProjectInfo(itype, info)
{
    console.log(itype + "> " + info);
}

var addon = require('./rklog_server');
addon.StartLogServer(out);
//addon.StopLogServer();