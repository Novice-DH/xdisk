#include "XFtpServerCMD.h"
#include <iostream>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <cstring>

using namespace std;

static void EventCB(struct bufferevent *bev, short what, void *arg)
{
    XFtpServerCMD *cmd = (XFtpServerCMD *)arg;

    // ���Է�����ϵ���������������쳣״̬���п����ղ��� BEV_EVENT_EOF ����
    if (what & (BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT))
    {
        cout << "BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT" << endl;
        bufferevent_free(bev);
        delete cmd; // ɾ������
    }
}

// ���߳� XThread event �¼��ַ�
static void ReadCB(struct bufferevent *bev, void *arg)
{
    XFtpServerCMD *cmd = (XFtpServerCMD *)arg;
    char data[1024] = {0};
    for (;;)
    {
        int len = bufferevent_read(bev, data, sizeof(data) - 1);
        if (len <= 0)
        {
            break;
        }
        data[len] = '\0'; // �������˵���Ǹ�һ���ַ���
        cout << data << flush;

        // test code, need to clean up
        // �� Windows cmd ���� �޷��˳�
        // �� ubuntu ���� �������˳�
        if (strstr(data, "quit"))
        {
            bufferevent_free(bev);
            delete cmd;
            break;
        }
    }
}

// ��ʼ������ ���������߳���
bool XFtpServerCMD::Init()
{
    cout << "XFtpServerCMD::Init()" << endl;
    // ���� socket���� bufferevent
    // ��Ҫ base��socket

    bufferevent *bev = bufferevent_socket_new(base, sock, BEV_OPT_CLOSE_ON_FREE); // �ر�ʱ���� socket
    bufferevent_setcb(bev, ReadCB, 0, EventCB, this);
    bufferevent_enable(bev, EV_READ | EV_WRITE); // ����Ȩ��

    // ���ӳ�ʱ���ƣ���ֹ�쳣״̬���п����ղ��� BEV_EVENT_EOF ����
    timeval rt = {10, 0};
    bufferevent_set_timeouts(bev, &rt, 0);

    return true;
}

XFtpServerCMD::XFtpServerCMD()
{
}

XFtpServerCMD::~XFtpServerCMD()
{
}
