#include "xupload_task.h"
#include <iostream>
// #include <cstring>

using namespace std;

// 接收到消息的回调
void XUploadTask::ReadCB(const XMsg *msg)
{
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
    WriteCB(&msg);
}
