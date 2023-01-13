#include "xcom_task.h"
#include <event2/bufferevent.h>
#include <event2/event.h>
#include <iostream>

using namespace std;

static void SEventCB(struct bufferevent *bev, short what, void *ctx)
{
    auto task = (XComTask *)ctx;
    task->EventCB(what);
}

void XComTask::EventCB(short what)
{
    cout << "EventCB: " << what << endl;
    if (what & BEV_EVENT_CONNECTED)
    {
        cout << "BEV_EVENT_CONNECTED" << endl;
        bufferevent_write(bev_, "OK", 3);
    }

    // 退出要处理缓冲内容（发送缓冲、读取缓冲）
    if (what & BEV_EVENT_ERROR || what & BEV_EVENT_TIMEOUT)
    {
        cout << "BEV_EVENT_ERROR or BEV_EVENT_TIMEOUT" << endl;
        // 清理资源，考虑缓冲区
        bufferevent_free(bev_);
    }

    if (what & BEV_EVENT_EOF)
    {
        cout << "BEV_EVENT_EOF" << endl;
        bufferevent_free(bev_);
    }
}

static void SWriteCB(struct bufferevent *bev, void *ctx)
{

}

static void SReadCB(struct bufferevent *bev, void *ctx)
{
    auto task = (XComTask *)ctx;
    task->ReadCB();
}

void XComTask::ReadCB()
{
    int len = bufferevent_read(bev_, read_buf_, sizeof(read_buf_));
    cout << read_buf_ << endl;
}


bool XComTask::Init()
{
    // 区分服务端、客户端
    int comsock = sock();
    if (comsock <= 0)
    {
        comsock = -1;
    }

    // 用 bufferevent 建立连接
    // 创建 bufferevent 上下文，-1 自动创建 socket
    bev_ = bufferevent_socket_new(base(), comsock, BEV_OPT_CLOSE_ON_FREE);
    if (!bev_)
    {
        cerr << "bufferevent_socket_new failed!" << endl;
        return false;
    }

    // 设置回调函数
    bufferevent_setcb(bev_, SReadCB, SWriteCB, SEventCB, this);
    bufferevent_enable(bev_, EV_READ | EV_WRITE); // 设置权限

    timeval tv = { 10,0 };
    bufferevent_set_timeouts(bev_, &tv, &tv);

    // 连接服务器
    // 若是服务端，则不用设置 server_ip，直接返回
    if (server_ip_.empty())
    {
        return true;
    }
    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(server_port_);
    evutil_inet_pton(AF_INET, server_ip_.c_str(), &sin.sin_addr.s_addr);
    int re = bufferevent_socket_connect(bev_, (sockaddr *)&sin, sizeof(sin));
    if (re != 0)
    {
        return false;
    }

    return true;
}