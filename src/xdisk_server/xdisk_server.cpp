#ifdef _WIN32
#include <windows.h>
#else
#include <signal.h>
#endif

#include "xthread_pool.h"
#include "xserver_task.h"
#include "xfile_server_task.h"
#include <iostream>
#include <thread>

using namespace std;

static void ListenCB(int sock, struct sockaddr *addr, int socklen, void *arg)
{
    auto task = new XFileServerTask();
    task->set_sock(sock);
    XThreadPool::Get()->Dispatch(task);
    cout << "ListenCB in main" << endl;
}

int main(int argc, char *argv[])
{
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
    // 获取端口、线程池线程数量
    int server_port = 21002;
    int thread_count = 10;
    if (argc > 1)
    {
        server_port = atoi(argv[1]);
    }
    if (argc > 2)
    {
        thread_count = atoi(argv[2]);
    }
    if (argc == 1)
    {
        cout << "xdisk_server server_port thread_count" << endl;
    }

    // 初始化 主线程池
    XThreadPool::Get()->Init(thread_count);

    // 专用于 server 连接的线程池
    XThreadPool server_pool;
    server_pool.Init(1);
    auto task = new XServerTask();
    task->set_server_port(server_port);
    task->ListenCB = ListenCB;
    server_pool.Dispatch(task);

    for (;;)
    {
        this_thread::sleep_for(1s);
    }

    return 0;
}