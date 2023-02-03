#include "xcom_task.h"
#include <event2/bufferevent.h>
#include <event2/event.h>
#include <iostream>
#include <cstring>

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
        // bufferevent_write(bev_, "OK", 3);
        // 通知连接建立成功
        ConnectedCB();
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

bool XComTask::WriteCB(const XMsg *msg)
{
    if (!bev_ || !msg || !msg->data || msg->size <= 0)
    {
        return false;
    }
    // 1 写入消息头
    int re = bufferevent_write(bev_, msg, sizeof(XMsgHead));
    if (re != 0)
    {
        return false;
    }
    // 2 写入消息内容
    re = bufferevent_write(bev_, msg->data, msg->size);
    if (re != 0)
    {
        return false;
    }
    return true;
}

static void SReadCB(struct bufferevent *bev, void *ctx)
{
    auto task = (XComTask *)ctx;
    task->ReadCB();
}

void XComTask::ReadCB(const XMsg *msg)
{
    cout << "recv Msg type= " << msg->type << " size= " << msg->size << endl;
}

// 接收数据包
void XComTask::ReadCB()
{
    for (;;) // 确保边缘触发能读取到 bufferevent 里所有数据
    {
        // 接收消息 XMsgHead
        // 1 接收头部消息
        if (!msg_.data)
        {
            int headsize = sizeof(XMsgHead); // 8 Byte
            int len = bufferevent_read(bev_, &msg_, headsize);
            if (!len) // 已读取完，但因为循环第 2 次进入，此时 len = 0
            {
                return;
            }
            if (len != headsize)
            {
                cerr << "msg head recv error!" << endl;
                return;
            }
        }
        // 2 验证消息头的有效性
        if (msg_.type >= MSG_MAX_TYPE || msg_.size <= 0 || msg_.size > MSG_MAX_SIZE)
        {
            cerr << "msg head is error!" << endl;
            return;
        }
        msg_.data = new char[msg_.size];

        int readsize = msg_.size - msg_.recved;
        if (readsize <= 0)
        {
            delete msg_.data;
            memset(&msg_, 0, sizeof(msg_));
            return;
        }

        int len = bufferevent_read(bev_, msg_.data + msg_.recved, readsize);
        if (len <= 0)
        {
            return;
        }
        msg_.recved += len;
        if (msg_.recved == msg_.size)
        {
            // 处理消息，释放空间
            cout << "recved msg: " << msg_.size << endl;
            ReadCB(&msg_);
            delete msg_.data;
            memset(&msg_, 0, sizeof(msg_));
        }
    }
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

    timeval tv = { 10, 0 };
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