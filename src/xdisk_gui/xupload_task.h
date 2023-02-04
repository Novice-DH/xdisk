#pragma once
#include "xcom_task.h"
#include <fstream>

// 定义回调函数类型
typedef void (*UploadCBFunc)();

class XUploadTask : public XComTask
{
public:
    // 接收到消息的回调
    virtual bool ReadCB(const XMsg *msg);

    // 连接成功的消息回调
    virtual void ConnectedCB();

    // 写入数据回调函数
    virtual void WriteCB();

    // 上传成功后的回调函数
    UploadCBFunc UploadCB = 0;

    void set_filepath(std::string path) { this->filepath_ = path; }

private:
    // 暂时不支持大于 2GB 的文件
    int filesize_ = 0;

    // 需要上传的文件路径
    std::string filepath_ = "";

    // 读取文件
    std::ifstream ifs_;
};
