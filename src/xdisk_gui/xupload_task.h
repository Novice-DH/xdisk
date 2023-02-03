﻿#pragma once
#include "xcom_task.h"
#include <fstream>

class XUploadTask : public XComTask
{
public:
    // 接收到消息的回调
    virtual void ReadCB(const XMsg *msg);
    // 连接成功的消息回调
    virtual void ConnectedCB();

    void set_filepath(std::string path) { this->filepath_ = path; }

private:
    // 需要上传的文件路径
    std::string filepath_ = "";

    // 读取文件
    std::ifstream ifs_;

    // 暂时不支持大于 2GB 的文件
    int filesize_ = 0;
};
