#pragma once
#include <event2/event.h>
#include <list>
#include <mutex>

class XTask;

class XThread
{
public:
    // �����߳�
    void Start();

    // �߳���ں���
    void Main();

    // ��װ�̣߳���ʼ�� event_base �� �ܵ������¼����ڼ���
    bool Setup();

    // �յ����̷߳����ļ�����Ϣ���̳߳صķַ���
    void Notify(evutil_socket_t fd, short which);

    // �̵߳ļ���
    void Activate();

    // ���Ӵ���������һ���߳̿���ͬʱ����������񣬹���һ�� event_base
    void AddTask(XTask *t);

    XThread();
    ~XThread();

    // �̱߳��
    int id = 0;

private:
    int notify_send_fd = 0;
    struct event_base *base = 0;

    // �����б�
    std::list<XTask *> tasks;

    // �̰߳�ȫ ����
    std::mutex tasks_mutex;
};
