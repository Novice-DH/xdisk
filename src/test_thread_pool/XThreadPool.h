#pragma once
#include <vector>

class XThread; // 只声明一下类型，而不去引用其头文件，防止多重引用
class XTask;

class XThreadPool
{
public:
    // 单件模式：只维系一个对象
    static XThreadPool* Get()
    {
        static XThreadPool p;
        return &p;
    }
    // 初始化所有线程，并启动线程
    void Init(int threadCount);

    // 分发线程
    void Dispatch(XTask* task);

private:
    // 线程数量
    int threadCount = 0;
    int lastThread = -1;
    // 线程池线程
    std::vector<XThread*>threads;
    XThreadPool() {};
};

