#pragma once
#include <vector>

class XThread; // ֻ����һ�����ͣ�����ȥ������ͷ�ļ�����ֹ��������
class XTask;

class XThreadPool
{
public:
    // ����ģʽ��ֻάϵһ������
    static XThreadPool *Get()
    {
        static XThreadPool p;
        return &p;
    }
    // ��ʼ�������̣߳��������߳�
    void Init(int threadCount);

    // �ַ��߳�
    void Dispatch(XTask *task);

private:
    // �߳�����
    int threadCount = 0;
    int lastThread = -1;
    // �̳߳��߳�
    std::vector<XThread *> threads;
    XThreadPool(){};
};
