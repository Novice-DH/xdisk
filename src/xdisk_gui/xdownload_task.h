#pragma once
#include "xcom_task.h"
#include <fstream>

class XDownLoadTask : public XComTask
{
public:
    // 接收到消息的回调
    virtual void ReadCB(const XMsg *msg);

    // 当关闭消息接收时，数据将发送到此函数，由业务模块重写
    virtual void ReadCB(void *data, int size);

    // 连接成功的消息回调
    virtual void ConnectedCB();

    void set_filepath(std::string path) { this->filepath_ = path; }

private:
    // 文件大小
    int filesize_ = 0;

    // 客户已下载的文件大小
    int recv_size_ = 0;

    // 目标文件相对路径
    std::string filepath_;

    // 写入接收的文件
    std::ofstream ofs_;
};
