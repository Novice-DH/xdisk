#ifndef XTHREAD_POOL_H
#define XTHREAD_POOL_H

#ifdef _WIN32
#ifdef XCOM_EXPORTS
#define XCOM_API __declspec(dllexport)
#else
#define XCOM_API __declspec(dllimport)
#endif
#else
#define XCOM_API
#endif

#include <vector>

class XThread; // 只声明一下类型，而不去引用其头文件，防止多重引用
class XTask;

// 若没有 XCOM_API 则只会生成 .dll，不会生成 .lib
class XCOM_API XThreadPool
{
public:
    /**
     * @brief 获取 XThreadPool 的静态对象（静态函数）
     *
     * @return XThreadPool*
     */
    static XThreadPool *Get()
    {
        static XThreadPool p;
        return &p;
    }

    /**
     * @brief 初始化所有线程，并启动线程，创建 event_base，并在线程中开始接收消息
     *
     * @param thread_count 线程数量
     */
    void Init(int thread_count);

    /**
     * @brief 分发任务到线程中执行，会调用 task 的 Init() 进行任务初始化
     *        任务会轮询分发到线程池种的各个线程
     *
     * @param task 任务接口对象，XTask 需要用户自己继承并重写 Init()
     */
    void Dispatch(XTask *task);

    XThreadPool(){};

private:
    // 线程数量
    int thread_count_ = 0;

    // 上一次分发得到的线程，用于轮询
    int last_thread_ = -1;

    // 线程池线程队列
    std::vector<XThread *> threads_;
};

#endif
