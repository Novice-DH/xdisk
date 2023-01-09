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

// �����߳�����Ļص�����
static void NotifyCB(evutil_socket_t fd, short which, void *arg)
{
    XThread *t = (XThread *)arg;
    t->Notify(fd, which);
}

// �յ����̷߳����ļ�����Ϣ���̳߳صķַ���
void XThread::Notify(evutil_socket_t fd, short which)
{
    // ��ȡ��Ϣ
    // ˮƽ������ֻҪû�н�����ɣ��ͻ��ٴν���
    char buf[2] = {0};
#ifdef _WIN32
    int re = recv(fd, buf, 1, 0);
#else
    // Linux ���� pipe�������� recv
    int re = read(fd, buf, 1);
#endif
    if (re <= 0)
    {
        return;
    }
    cout << id << " thread " << buf << endl;

    XTask *task = nullptr;
    // ��ȡ���񣬲���ʼ������
    tasks_mutex.lock();
    if (tasks.empty())
    {
        tasks_mutex.unlock(); // ע�⣬return ֮ǰ���� Ҫ�ͷ�
        return;
    }
    task = tasks.front(); // �Ƚ��ȳ�
    tasks.pop_front();
    tasks_mutex.unlock();
    task->Init();
}

// ���Ӵ���������һ���߳̿���ͬʱ����������񣬹���һ�� event_base
void XThread::AddTask(XTask *t)
{
    if (!t)
    {
        return;
    }
    // �� Dispatch �У���֪���߳� base����Ҫ����
    // ������ base �������¼�
    t->base = this->base;
    tasks_mutex.lock();
    tasks.push_back(t);
    tasks_mutex.unlock();
}

// �̵߳ļ���
void XThread::Activate()
{
#ifdef _WIN32
    int re = send(this->notify_send_fd, "c", 1, 0);
#else
    // Linux ���� pipe�������� send
    int re = write(this->notify_send_fd, "c", 1);
#endif
    if (re <= 0)
    {
        cerr << "XThread::Activate() failed!" << endl;
    }
}

// �����߳�
void XThread::Start()
{
    Setup();
    // �����߳�
    // th ���������� Start ��
    // �ص������ĵ�ַ��Main �ǳ�Ա�������ö������� this ����
    thread th(&XThread::Main, this);

    // ���̻߳ᱣ��һ������Դ�����߳�����ϵ
    // th ����֮���������
    //
    // �������̣߳��Ͽ������̵߳���ϵ��
    // ��ʹ th ���� �봴���õ��߳�Ҳû�й�ϵ��
    th.detach();
}

// ��װ�̣߳���ʼ�� event_base �� �ܵ������¼����ڼ���
bool XThread::Setup()
{
    // Windows �� socketpair��Linux �� pipe
#ifdef _WIN32
    // ����һ�� socketpair�����Ի���ͨ�ţ�fds[0] ����fds[1] д
    evutil_socket_t fds[2];
    if (evutil_socketpair(AF_INET, SOCK_STREAM, 0, fds) < 0)
    {
        cout << "evutil_socketpair failed!" << endl;
        return false;
    }
    // ���óɷ�����
    evutil_make_socket_nonblocking(fds[0]);
    evutil_make_socket_nonblocking(fds[1]);
#else
    // ������ pipe���� read write ��ȡ
    // ������ send, recv ��ȡ��socket ���ã�
    int fds[2];
    if (pipe(fds))
    {
        cerr << "pipe failed!" << endl;
        return false;
    }
#endif

    // ��ȡ�󶨵� event �¼��У�д��Ҫ����
    notify_send_fd = fds[1];

    // ���� libevent �����ģ����������ο��� memcached
    event_config *ev_conf = event_config_new();
    event_config_set_flag(ev_conf, EVENT_BASE_FLAG_NOLOCK);
    this->base = event_base_new_with_config(ev_conf);
    event_config_free(ev_conf);
    if (!base)
    {
        cerr << "event_base_new_with_config failed in thread!" << endl;
        return false;
    }

    // ���� pipe �����¼������ڼ����߳�ִ������
    // this ��ʾ��ǰ����ĵ�ַ
    event *ev = event_new(base, fds[0], EV_READ | EV_PERSIST, NotifyCB, this);
    event_add(ev, 0);

    return true;
}

// �߳���ں���
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
