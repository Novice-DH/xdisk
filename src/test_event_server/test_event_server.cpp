//#define _CRT_SECURE_NO_WARNINGS
#include <event2/event.h>
#include <iostream>

#ifdef _WIN32
#else
#include <signal.h>
#endif
using namespace std;

void ListenCB(evutil_socket_t sock, short what, void* arg) {
    cout << "ListenCB" << endl;
    if (!(what & EV_READ)) {
        cout << "not read" << endl;
        return;
    }
    sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    socklen_t size = sizeof(sin);
    evutil_socket_t client_socket = accept(sock, (sockaddr*)&sin, &size);
    if (client_socket <= 0) {
        cerr << "accept error!" << endl;
        return;
    }
    char ip[16] = { 0 };
    evutil_inet_ntop(AF_INET, &sin.sin_addr, ip, sizeof(ip));
    cout << "client ip is " << ip << endl;
}

int main(int argc, char *argv[])
{
    int server_port = 20010;
    if (argc > 1) {
        server_port = atoi(argv[1]);
    }
#ifdef _WIN32
    //初始化 socket 库
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
#else
    // Linux 使用断开连接的 socket
    // 会发出此信号，造成程序退出
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        return 1;
    }
#endif
     
    std::cout << "test event server!\n";
    // 1 创建 libevent 的上下文
    // 默认创建 base 锁，是线程安全的
    event_base * base = event_base_new();
    if (base) {
        cout << "event_base_new success!" << endl;
    }

    // 2 创建 socket，绑定 port
    evutil_socket_t sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock <= 0) {
        // _CRT_SECURE_NO_WARNINGS
        cerr << "socket error" << strerror(errno) << endl;
        return -1;
    }

    // 设置地址复用、非阻塞
    evutil_make_socket_nonblocking(sock);
    evutil_make_listen_socket_reuseable(sock);

    // 绑定端口
    //sockaddr sin; // err
    sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(server_port);
    //sin.sin_addr.s_addr = htonl(0);
    int re = ::bind(sock, (sockaddr*)&sin, sizeof(sin));
    if (re != 0) {
        // _CRT_SECURE_NO_WARNINGS
        cerr << "bind port:" << server_port <<" error!" << strerror(errno) << endl;
        return -1;
    }
    listen(sock, 10);
    cout << "bind port:" << server_port << " success!" << endl;

    // 3 注册 socket 的监听事件回调函数
    // EV_PERSIST 持久化，不然只进入一次事件
    // EV_ET：边缘出发，默认为 水平触发
    // event_self_cbarg() 传递当前创建的 event 对象
    event *ev = event_new(base, sock, EV_READ | EV_PERSIST, ListenCB, event_self_cbarg());
    // 开始监听事件
    // 第2个参数：超时时间
    event_add(ev, 0);

    // 事件主循环：监控事件是否发生，分发事件到回调函数
    // 若没有事件注册，则退出
    event_base_dispatch(base);

    // 清理 mallco 空间
    evutil_closesocket(sock);
    event_del(ev); // 先从事件队列里清理，防止删除正在调用的事件
    event_free(ev);

    // 清理base下的所有资源，其实是包含 上面3步的
    event_base_free(base);

    return 0;
}
