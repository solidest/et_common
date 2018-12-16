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

class RklogClient
{
    public:
        ~RklogClient();
        RklogClient(const char* clientname, const char* serverip, unsigned short serverport);

        void LogInfo(const std::string& info);
        void LogError(const std::string& info);
        void LogWarning(const std::string& info);
        void LogDebug(const std::string& info);

    private:
        rpc::client* _c;
        std::string _name;
        inline void log(const std::string& itype, const std::string& info);   
};


#endif /* !RKLOG_CLIENT_H_ */
