#include "XThread.h"
#include "XTask.h"
#include <iostream>
#include <thread>
#include <event2/event.h>

#ifdef _WIN32
#else
#include <unistd.h>
#endif
using namespace std;

// 激活线程任务的回调函数
static void NotifyCB(evutil_socket_t fd, short which, void* arg)
{
    XThread* t = (XThread*)arg;
    t->Notify(fd, which);
}

// 收到主线程发出的激活消息（线程池的分发）
void XThread::Notify(evutil_socket_t fd, short which)
{
    // 读取消息
    // 水平触发，只要没有接收完成，就会再次进来
    char buf[2] = { 0 };
#ifdef _WIN32
    int re = recv(fd, buf, 1, 0);
#else
    // Linux 中是 pipe，不能用 recv
    int re = read(fd, buf, 1);
#endif
    if (re <= 0)
    {
        return;
    }
    cout << id << " thread " << buf << endl;

    XTask* task = nullptr;
    // 获取任务，并初始化任务
    tasks_mutex.lock();
    if (tasks.empty())
    {
        tasks_mutex.unlock(); // 注意，return 之前，锁 要释放
        return;
    }
    task = tasks.front(); // 先进先出
    tasks.pop_front();
    tasks_mutex.unlock();
    task->Init();
}

// 添加处理的任务，一个线程可以同时处理多个任务，共用一个 event_base
void XThread::AddTask(XTask* t)
{
    if (!t) { return; }
    // 在 Dispatch 中，不知道线程 base，故要传递
    // 可以往 base 里添加事件
    t->base = this->base;
    tasks_mutex.lock();
    tasks.push_back(t);
    tasks_mutex.unlock();
}

// 线程的激活
void XThread::Activate()
{
#ifdef _WIN32
    int re = send(this->notify_send_fd, "c", 1, 0);
#else
    // Linux 中是 pipe，不能用 send
    int re = write(this->notify_send_fd, "c", 1);
#endif
    if (re <= 0)
    {
        cerr << "XThread::Activate() failed!" << endl;
    }
}

// 启动线程
void XThread::Start()
{
    Setup();
    // 启动线程
    // th 生命周期在 Start 内
    // 回调函数的地址，Main 是成员函数，用对象本身的 this 访问
    thread th(&XThread::Main, this);

    // 主线程会保留一部分资源与子线程相联系
    // th 销毁之后会有问题
    // 
    // 清理主线程（断开与主线程的联系）
    // 即使 th 销毁 与创建好的线程也没有关系了
    th.detach();
}

// 安装线程，初始化 event_base 和 管道监听事件用于激活
bool XThread::Setup()
{
    // Windows 用 socketpair，Linux 用 pipe
#ifdef _WIN32
    // 创建一个 socketpair，可以互相通信，fds[0] 读，fds[1] 写
    evutil_socket_t fds[2];
    if (evutil_socketpair(AF_INET, SOCK_STREAM, 0, fds) < 0)
    {
        cout << "evutil_socketpair failed!" << endl;
        return false;
    }
    // 设置成非阻塞
    evutil_make_socket_nonblocking(fds[0]);
    evutil_make_socket_nonblocking(fds[1]);
#else
    // 创建的 pipe，用 read write 读取
    // 不能用 send, recv 读取（socket 才用）
    ind fds[2];
    if (pipe(fds))
    {
        cerr << "pipe failed!" << endl;
        return false;
}
#endif

    // 读取绑定到 event 事件中，写入要保存
    notify_send_fd = fds[1];

    // 创建 libevent 上下文（无锁），参考自 memcached
    event_config* ev_conf = event_config_new();
    event_config_set_flag(ev_conf, EVENT_BASE_FLAG_NOLOCK);
    this->base = event_base_new_with_config(ev_conf);
    event_config_free(ev_conf);
    if (!base)
    {
        cerr << "event_base_new_with_config failed in thread!" << endl;
        return false;
    }

    // 添加 pipe 监听事件，用于激活线程执行任务
    // this 表示当前对象的地址
    event* ev = event_new(base, fds[0], EV_READ | EV_PERSIST, NotifyCB, this);
    event_add(ev, 0);

    return true;
}

// 线程入口函数
void XThread::Main()
{
    cout << id << " XThread::Main() begin!" << endl;
    event_base_dispatch(base);
    event_base_free(base);
    cout << id << " XThread::Main() end!" << endl;
}

XThread::XThread()
{

}

XThread::~XThread()
{

}
