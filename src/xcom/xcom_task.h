
#ifndef XCOM_TASK_H
#define XCOM_TASK_H

#include "xtask.h"
#include <string>

class XCOM_API XComTask : public XTask
{
public:
    bool Init();

    void set_server_ip(std::string ip) { this->server_ip_ = ip; }
    void set_port(int port) { this->server_port_ = port; }

private:
    // 服务器 IP
    std::string server_ip_ = "";

    // 服务器端口
    int server_port_ = 0;

    struct bufferevent *bev_ = 0;
};

#endif
