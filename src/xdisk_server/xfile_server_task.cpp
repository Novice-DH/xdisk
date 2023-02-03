#include "xfile_server_task.h"
#include "xmsg.h"
#include "xtools.h"
#include <iostream>
#include <string>

using namespace std;

void XFileServerTask::ReadCB(const XMsg *msg)
{
    switch (msg->type)
    {
    case MSG_GETDIR:
        cout << "MSG_GETDIR" << endl;
        GetDir(msg);
        break;
    case MSG_UPLOAD_INFO:
        cout << "MSG_UPLOAD_INFO" << endl;
    default:
        break;
    }
}

// 处理目录获取的消息，返回目录列表
void XFileServerTask::GetDir(const XMsg *msg)
{
    if (!msg->data)
    {
        return;
    }
    string root = msg->data;
    if (root.empty())
    {
        root = "./";
    }
    // string dir = "file1,1024;file2,4096;file3.zip,10240";
    string dir = GetDirData(root);
    XMsg resmsg;
    resmsg.type = MSG_DIRLIST;
    resmsg.size = dir.size() + 1; // +1 发送 \0
    resmsg.data = (char *)dir.c_str();
    WriteCB(&resmsg);
}