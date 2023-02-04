#include "xupload_task.h"
#include <iostream>

using namespace std;

// 接收到消息的回调
bool XUploadTask::ReadCB(const XMsg *msg)
{
    switch (msg->type)
    {
    case MSG_UPLOAD_ACCEPT:
        BeginWrite();
        break;
    case MSG_UPLOAD_COMPLETE:
        // 界面刷新
        if (UploadCB)
        {
            UploadCB();
        }
        break;
    default:
        break;
    }
    return true;
}

// 写入数据回调函数
void XUploadTask::WriteCB()
{
    if (!ifs_.is_open())
    {
        return;
    }
    ifs_.read(read_buf_, sizeof(read_buf_));
    int len = ifs_.gcount(); // 读取的长度
    if (len <= 0)
    {
        ifs_.close();
        return;
    }
    Write(read_buf_, len);
    if (ifs_.eof()) // 文件已读到结尾
    {
        ifs_.close();
    }
}

// 连接成功的消息回调
void XUploadTask::ConnectedCB()
{
    if (filepath_.empty())
    {
        cerr << "XUploadTask::ConnectedCB filepath_ is empty!" << endl;
        return;
    }
    // 1 确保能打开本地文件
    // 打开文件，跳转到结尾，以获取文件大小
    ifs_.open(filepath_.c_str(), ios::in | ios::binary | ios::ate);
    if (!ifs_.is_open())
    {
        cerr << "open file " << filepath_ << " failed!" << endl;
        return;
    }
    // 2 获取文件名称、大小 filename.zip,1024
    filesize_ = ifs_.tellg();
    ifs_.seekg(0, ios::beg);
    cout << "open file " << filepath_ << " success!" << endl;
    string filename = "";
    int pos = filepath_.find_last_of('/');
    if (pos < 0)
    {
        pos = 0;
    }
    filename = filepath_.substr(pos, filepath_.size() - pos);
    char data[1024] = {0};
    sprintf(data, "%s,%d", filename.c_str(), filesize_);

    // 3 发送上传文件请求
    XMsg msg;
    msg.type = MSG_UPLOAD_INFO;
    msg.data = data;
    msg.size = strlen(data) + 1;
    Write(&msg);
}
