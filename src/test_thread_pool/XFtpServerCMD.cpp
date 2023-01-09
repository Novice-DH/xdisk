#include "XFtpServerCMD.h"
#include <iostream>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <cstring>

using namespace std;

static void EventCB(struct bufferevent *bev, short what, void *arg)
{
    XFtpServerCMD *cmd = (XFtpServerCMD *)arg;

    // 若对方网络断掉，或机器死机（异常状态）有可能收不到 BEV_EVENT_EOF 数据
    if (what & (BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT))
    {
        cout << "BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT" << endl;
        bufferevent_free(bev);
        delete cmd; // 删除对象
    }
}

// 子线程 XThread event 事件分发
static void ReadCB(struct bufferevent *bev, void *arg)
{
    XFtpServerCMD *cmd = (XFtpServerCMD *)arg;
    char data[1024] = {0};
    for (;;)
    {
        int len = bufferevent_read(bev, data, sizeof(data) - 1);
        if (len <= 0)
        {
            break;
        }
        data[len] = '\0'; // 代码层面说明是赋一个字符串
        cout << data << flush;

        // test code,need to clean up
        // 用 Windows cmd 连接 无法退出
        // 用 ubuntu 连接 可正常退出
        if (strstr(data, "quit"))
        {
            bufferevent_free(bev);
            delete cmd;
            break;
        }
    }
}

// 初始化任务 运行在子线程中
bool XFtpServerCMD::Init()
{
    cout << "XFtpServerCMD::Init()" << endl;
    // 监听 socket，用 bufferevent
    // 需要 base、socket

    bufferevent *bev = bufferevent_socket_new(base, sock, BEV_OPT_CLOSE_ON_FREE); // 关闭时清理 socket
    bufferevent_setcb(bev, ReadCB, 0, EventCB, this);
    bufferevent_enable(bev, EV_READ | EV_WRITE); // 设置权限

    // 添加超时机制（防止异常状态）有可能收不到 BEV_EVENT_EOF 数据
    timeval rt = {10, 0};
    bufferevent_set_timeouts(bev, &rt, 0);

    return true;
}

XFtpServerCMD::XFtpServerCMD()
{
}

XFtpServerCMD::~XFtpServerCMD()
{
}
