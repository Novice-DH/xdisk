﻿#ifndef XSERVER_TASK_H
#define XSERVER_TASK_H

#include "xtask.h"

typedef void (*ListenCBFunc)(int sock, struct sockaddr *addr, int socklen, void *arg);

class XCOM_API XServerTask : public XTask
{
public:
    virtual bool Init();
    ListenCBFunc ListenCB = 0; // 函数指针，可以直接调用
    void set_server_port(int port) { this->server_port_ = port; }

private:
    int server_port_;
};

#endif
