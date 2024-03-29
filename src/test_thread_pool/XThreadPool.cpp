﻿#include "XThreadPool.h"
#include "XThread.h"
#include "XTask.h"
#include <thread>
#include <iostream>

using namespace std;

// 分发线程
void XThreadPool::Dispatch(XTask *task)
{
    // 轮询 找到 合适线程
    if (!task) { return; }
    int tid = (lastThread + 1) % threadCount;
    lastThread = tid;
    XThread *t = threads[tid];

    t->AddTask(task);

    // 激活线程
    t->Activate();
}

// 初始化所有线程，并启动线程
void XThreadPool::Init(int threadCount)
{
    this->threadCount = threadCount; // 参数名不与成员名一致
    this->lastThread = -1;
    for (int i = 0; i < threadCount; ++i)
    {
        XThread *t = new XThread();
        t->id = i + 1;
        cout << "Create thread " << i << endl;
        // 启动线程
        t->Start();
        threads.push_back(t);
        this_thread::sleep_for(10ms);
    }
}
