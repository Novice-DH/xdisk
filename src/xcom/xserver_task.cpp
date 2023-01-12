#include "xserver_task.h"
#include <event2/event.h>
#include <event2/listener.h>
#include <iostream>
#include <cstring> // memset() on Linux

using namespace std;

static void SListenCB(struct evconnlistener *evc, evutil_socket_t client_socket, struct sockaddr *client_addr, int socklen, void *arg)
{
    cout << "SListenCB" << endl;
    auto task = (XServerTask *)arg;
    if (task->ListenCB)
    {
        task->ListenCB(client_socket, client_addr, socklen, arg);
    }
    else
    {
        cerr << "please set callback function ListenCB" << endl;
    }
}

bool XServerTask::Init()
{
    if (server_port_ <= 0)
    {
        cerr << "XServerTask::Init() failed! server_port_ is not set." << endl;
        return false;
    }
    // 监听端口
    sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(server_port_);

    // 设置回调函数
    auto evc = evconnlistener_new_bind(base(), SListenCB, this,
                                       LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
                                       10,
                                       (sockaddr *)&sin,
                                       sizeof(sin));
    if (!evc)
    {
        cout << "listen port " << server_port_ << " failed!" << endl;
        return false;
    }
    cout << "listen port " << server_port_ << " success!" << endl;

    return true;
}