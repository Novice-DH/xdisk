#include "xfile_server_task.h"
#include "xmsg.h"
#include "xtools.h"
#include <iostream>
#include <string>

using namespace std;

string XFileServerTask::cur_dir_ = "./";
mutex XFileServerTask::cur_dir_mux_;

bool XFileServerTask::ReadCB(const XMsg *msg)
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
    case MSG_DOWNLOAD_INFO:
        cout << "MSG_DOWNLOAD_INFO" << endl;
        Download(msg);
        break;
    case MSG_DOWNLOAD_COMPLETE:
        cout << "MSG_DOWNLOAD_COMPLETE" << endl;
        // 清理网络资源
        Close();
        return false;
        break;
    default:
        break;
    }
    return true;
}

// 当关闭消息接收时，数据将发送到此函数，由业务模块重写
void XFileServerTask::ReadCB(void *data, int size)
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
        resmsg.type = MSG_UPLOAD_COMPLETE;
        resmsg.size = 3; // +1 发送 \0
        resmsg.data = (char *)"OK";
        Write(&resmsg); // 发完后，激发 WriteCB
    }
}

// 写入数据回调函数
void XFileServerTask::WriteCB()
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
    if (!msg || !msg->data || msg->size <= 0)
    {
        return;
    }
    // 1 获取文件名、文件大小
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

    // 3 回复 MSG_UPLOAD_ACCEPT
    XMsg resmsg;
    resmsg.type = MSG_UPLOAD_ACCEPT;
    resmsg.size = 3; // +1 发送 \0
    resmsg.data = (char *)"OK";
    Write(&resmsg);

    // 不再接收消息，开始接收文件
    set_is_recv_msg(false);
    recv_size_ = 0; // 每次打开后置0
}

// 处理客户端的下载请求
void XFileServerTask::Download(const XMsg *msg)
{
    if (!msg || !msg->data || msg->size <= 0)
    {
        return;
    }
    // 1 打开文件，跳转到结尾，以获取文件大小
    filepath_ = msg->data;
    if (filepath_.empty())
    {
        return;
    }

    // 2 获取文件名、大小 filename.zip,1024
    ifs_.open(filepath_.c_str(), ios::in | ios::binary | ios::ate);
    if (!ifs_.is_open())
    {
        cerr << "open file " << filepath_ << " failed!" << endl;
        return;
    }
    filesize_ = ifs_.tellg();
    ifs_.seekg(0, ios::beg);
    cout << "open file " << filepath_ << " success!" << endl;

    // 3 回复 MSG_DOWNLOAD_ACCEPT
    char buf[32] = {0};
    // use _CRT_SECURE_NO_WARNINGS
    sprintf(buf, "%d", filesize_);
    XMsg resmsg;
    resmsg.type = MSG_DOWNLOAD_ACCEPT;
    resmsg.size = strlen(buf) + 1; // +1 发送 \0
    resmsg.data = buf;             // 告知客户端 文件大小
    Write(&resmsg);
}
