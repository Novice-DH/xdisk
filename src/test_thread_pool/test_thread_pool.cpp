#include "XThreadPool.h"
#include "XFtpServerCMD.h"
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <iostream>

#define SPORT 5001

#ifdef _WIN32
#else
#include <signal.h>
#include <cstring>
#endif
using namespace std;

// void EventCB(struct bufferevent *bev, short what, void *ctx)
// {
//     // 有事件进入
//     cout << "[E]" << endl;
//     // 读超时
//     if (what & BEV_EVENT_TIMEOUT && what & BEV_EVENT_READING)
//     {
//         cout << "READING BEV_EVENT_TIMEOUT" << endl;
//         // 读取完缓冲内剩余内容

//         // 清理空间，关闭监听
//         bufferevent_free(bev);
//     }
//     // 写超时
//     else if (what & BEV_EVENT_TIMEOUT && what & BEV_EVENT_WRITING)
//     {
//         cout << "WRITING BEV_EVENT_TIMEOUT" << endl;
//         // 缓冲回滚

//         // 清理空间，关闭监听
//         bufferevent_free(bev);
//     }
//     // 异常错误
//     else if (what & BEV_EVENT_ERROR)
//     {
//         cout << "BEV_EVENT_ERROR" << endl;
//         // 处理缓冲内容（读 、回滚）

//         // 清理空间，关闭监听
//         bufferevent_free(bev);
//     }
//     // 连接断开（正常断开，对方先关闭连接）
//     else if (what & BEV_EVENT_EOF)
//     {
//         cout << "BEV_EVENT_EOF" << endl;
//         // 考虑缓冲的处理（ReadCB 还未被调用，EventCB 先被激发）

//         // 清理空间，关闭监听
//         bufferevent_free(bev);
//     }
// }

// // [W] → OK → [W]
// // 可见 OK 发完之后（可写时机），会再次激发 WriteCB 调用
// void WriteCB(struct bufferevent *bev, void *ctx)
// {
//     cout << "[W]" << endl;
// }

// void ReadCB(struct bufferevent *bev, void *ctx)
// {
//     cout << "+" << flush;
//     char buf[1024] = { 0 };
//     int len = bufferevent_read(bev, buf, sizeof(buf) - 1);
//     cout << buf << endl;

//     // 将数据插入 buffer 链表（此时还未发出去），等到可写时再处理
//     bufferevent_write(bev, "OK", 3);
// }

void ListenCB(struct evconnlistener *evc, evutil_socket_t client_socket, struct sockaddr *client_addr, int socklen, void *arg)
{
    cout << "ListenCB" << endl;
    XTask *task = new XFtpServerCMD();
    task->sock = client_socket;
    XThreadPool::Get()->Dispatch(task);
}

int main(int argc, char *argv[])
{
    // int server_port = 20010;
    // if (argc > 1)
    // {
    //     server_port = atoi(argv[1]);
    // }
#ifdef _WIN32
    // 初始化 socket 库
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
#else
    // Linux 使用断开连接的 socket 会发出此信号，造成程序退出
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
    {
        return 1;
    }
#endif
    // 1 初始化线程池
    XThreadPool::Get()->Init(10);

    std::cout << "test thread pool!\n";
    // 1 创建 libevent 的上下文
    // 默认创建 base 锁，是线程安全的
    event_base *base = event_base_new();
    if (base)
    {
        cout << "event_base_new success!" << endl;
    }

    // 监听 port
    // socket bind listen 绑定事件
    sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(SPORT);

    // 第 3 个参数及之后，是传入 回调函数 的值
    auto evc = evconnlistener_new_bind(base,                                      // libevent 的上下文
                                       ListenCB,                                  // 接收到连接的回调函数
                                       base,                                      // 回调函数获取的参数 arg
                                       LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, // 地址重用，evconnlistener 关闭时清理
                                       10,                                        // listen 缓冲大小
                                       (sockaddr *)&sin,
                                       sizeof(sin));

    // 事件主循环：监控事件是否发生，分发事件到回调函数
    // 若没有事件注册，则退出
    event_base_dispatch(base);
    evconnlistener_free(evc); // evc 资源的清理
    // 清理base下的所有资源，其实是包含 上面3步的
    event_base_free(base);

    return 0;
}
