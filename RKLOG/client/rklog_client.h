/*
** rklog_client.h for et_common in /home/byx/et/et_common/RKLOG/client
**
** Made by header_template
** Login   <>
**
** Started on  Fri Nov 30 10:14:21 PM 2018 header_template
** Last update Sat Nov 30 10:20:28 PM 2018 header_template
*/

#ifndef RKLOG_CLIENT_H_
# define RKLOG_CLIENT_H_

#include <string>
#include "rpc/client.h"

class rkLog
{
    private:
        rpc::client* _c;

    public:
        ~rkLog();
        rkLog(const char* serverip, unsigned short serverport);
        int log(std::string info);   
};


#endif /* !RKLOG_CLIENT_H_ */
