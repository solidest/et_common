

function showProjectInfo(pjinfolist)
{
  var res = JSON.parse(pjinfolist)
  res.forEach(function(value, index, array) {
    var info = JSON.parse(value.value);
    console.log((index+1) + " of " + array.length);
    console.log("Key: " + value.key);
    console.log(new Date(info.CreateTime));
    console.log(info.ProjectInfo);
    console.log(new Date(info.UpdateTime));
    console.log('\n');
  });
}

var addon = require('./rkdb_client');
addon.OpenRkdb("127.0.0.1", 8000);
showProjectInfo(addon.GetProjectInfoList());
var newid = addon.NewProjectInfo("new pjinfo from js!");
showProjectInfo(addon.GetProjectInfoList());
addon.SetProjectInfo(newid, "中文字符update pjinfo to newinfo!!")
showProjectInfo(addon.GetProjectInfoList());

var scont1 = "content of project";
addon.SetProject(newid, scont1);
var scont = addon.GetProject(newid);
console.log(scont);

var scont2 = "中文字符new content!@#";
addon.SetProject(newid, scont2);
scont = addon.GetProject(newid);
console.log(scont);

addon.DelProject(newid);
showProjectInfo(addon.GetProjectInfoList());
addon.CloseRkdb();
console.log("passed!");