#include "xdownload_task.h"
#include <iostream>

using namespace std;

// 接收到消息的回调
void XDownLoadTask::ReadCB(const XMsg *msg)
{
    switch (msg->type)
    {
    case MSG_DOWNLOAD_ACCEPT:
        // 不再接收消息，开始接收文件
        set_is_recv_msg(false);
        recv_size_ = 0; // 每次打开后置0
        break;
    default:
        break;
    }
}

// 当关闭消息接收时，数据将发送到此函数，由业务模块重写
void XDownLoadTask::ReadCB(void *data, int size)
{
    if (!data || size <= 0 || !ofs_.is_open())
    {
        return;
    }
    ofs_.write((char *)data, size);
    recv_size_ += size;
    if (recv_size_ == filesize_)
    {
        cout << "file write end!" << endl;
        ofs_.close();
        XMsg resmsg;
        resmsg.type = MSG_DOWNLOAD_COMPLETE;
        resmsg.size = 3; // +1 发送 \0
        resmsg.data = (char *)"OK";
        Write(&resmsg); // 发完后，激发 WriteCB
    }
}

// 连接成功的消息回调
void XDownLoadTask::ConnectedCB()
{
    if (filepath_.empty())
    {
        Close();
        return;
    }
    // 3 发送下载文件请求
    string data = filepath_;

    // ofs_.open(filepath, ios::out || ios::binary); // err
    ofs_.open(filepath_, ios::out | ios::binary);
    if (!ofs_.is_open())
    {
        cout << "open file " << filepath_ << " failed!" << endl;
        return;
    }
    cout << "open file " << filepath_ << " success!" << endl;

    XMsg msg;
    msg.type = MSG_DOWNLOAD_INFO;
    msg.data = (char *)data.c_str();
    msg.size = data.size() + 1;
    Write(&msg);
}