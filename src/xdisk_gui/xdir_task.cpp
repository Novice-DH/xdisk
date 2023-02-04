#include "xdir_task.h"
#include <iostream>

using namespace std;

bool XDirTask::ReadCB(const XMsg *msg)
{
    // 接收到服务端发送的目录
    switch (msg->type)
    {
    case MSG_DIRLIST: // 服务器返回的目录列表
        cout << "MSG_DIRLIST" << endl;
        DirCB(msg->data);
        break;

    default:
        break;
    }
    return true;
}

// 连接成功的消息回调
void XDirTask::ConnectedCB()
{
    XMsg msg;
    msg.type = MSG_GETDIR;
    msg.size = server_root_.size() + 1;
    msg.data = (char *)server_root_.c_str();
    Write(&msg);
}
