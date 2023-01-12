#ifndef XTHREAD_H
#define XTHREAD_H

#include <list>
#include <mutex>

class XTask;

class XThread
{
public:
    // 启动线程
    void Start();

    // 线程入口函数
    void Main();

    // 安装线程，初始化 event_base 和 管道监听事件用于激活
    bool Setup();

    // 收到主线程发出的激活消息（线程池的分发）
    void Notify(int fd, short which);

    // 线程的激活
    void Activate();

    // 添加处理的任务，一个线程可以同时处理多个任务，共用一个 event_base
    void AddTask(XTask *t);

    XThread();
    ~XThread();

    // 线程编号
    int id = 0;

private:
    int notify_send_fd_ = 0;
    struct event_base *base_ = 0;

    // 任务列表
    std::list<XTask *> tasks_;

    // 线程安全 互斥
    std::mutex tasks_mutex_;
};

#endif
