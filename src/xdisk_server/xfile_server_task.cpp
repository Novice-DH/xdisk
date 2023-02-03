#include "xfile_server_task.h"
#include "xmsg.h"
#include "xtools.h"
#include <iostream>
#include <string>

using namespace std;

string XFileServerTask::cur_dir_ = "./";
mutex XFileServerTask::cur_dir_mux_;

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
        Upload(msg);
        break;
    default:
        break;
    }
}

// 当关闭消息接收时，数据将发送到此函数，由业务模块重写
void XFileServerTask::ReadCB(void *data, int size)
{
    if (!data || size <= 0 || !ofs_.is_open())
    {
        return;
    }
    ofs_.write((char *)data, size);
    upload_size_ += size;
    if (upload_size_ == filesize_)
    {
        cout << "file write end!" << endl;
        ofs_.close();
        XMsg resmsg;
        resmsg.type = MSG_UPLOAD_COMPLETE;
        resmsg.size = 3; // +1 发送 \0
        resmsg.data = (char *)"OK";
        Write(&resmsg);
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
    set_cur_dir(root);
    // string dir = "file1,1024;file2,4096;file3.zip,10240";
    string dir = GetDirData(root);
    XMsg resmsg;
    resmsg.type = MSG_DIRLIST;
    resmsg.size = dir.size() + 1; // +1 发送 \0
    resmsg.data = (char *)dir.c_str();
    Write(&resmsg);
}

// 处理客户端的上传请求
void XFileServerTask::Upload(const XMsg *msg)
{
    // 1 获取文件名、文件大小
    if (!msg || !msg->data || msg->size <= 0)
    {
        return;
    }
    string str = msg->data;
    if (str.empty())
    {
        return;
    }
    // filename.zip,1024
    int pos = str.find_last_of(',');
    if (pos <= 0)
    {
        return;
    }
    string filename = str.substr(0, pos);
    int sizepos = pos + 1;
    if (sizepos >= str.size())
    {
        return;
    }
    string tmp = str.substr(sizepos, str.size() - sizepos);
    cout << filename << ":" << tmp << endl;
    filesize_ = atoi(tmp.c_str());
    if (filesize_ <= 0)
    {
        return;
    }

    // 2 打开本地文件
    string filepath = cur_dir() + filename;
    // ofs_.open(filepath, ios::out || ios::binary); // err
    ofs_.open(filepath, ios::out | ios::binary);
    if (!ofs_.is_open())
    {
        cout << "open file " << filepath << " failed!" << endl;
        return;
    }
    cout << "open file " << filepath << " success!" << endl;

    // 3 回复 accept
    XMsg resmsg;
    resmsg.type = MSG_UPLOAD_ACCEPT;
    resmsg.size = 3; // +1 发送 \0
    resmsg.data = (char *)"OK";
    Write(&resmsg);

    // 不再接收消息，开始接收文件
    set_is_recv_msg(false);
    upload_size_ = 0; // 每次打开后置0
}