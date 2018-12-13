
var addon = require('./rkdb_client');
var res = JSON.parse(addon.GetProjectInfoList());
res.forEach(function(value, index, array) {
    var info = JSON.parse(value.value);
    console.log(index);
    console.log(value.key);
    console.log(new Date(info.CreateTime));
    console.log(info.ProjectInfo);
    var dt1 = new Date(info.UpdateTime);
    console.log(dt1);
    console.log('\n');
  });