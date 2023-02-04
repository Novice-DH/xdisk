#pragma once
#include "xcom_task.h"

// 定义回调函数类型
typedef void (*DirCBFunc)(std::string dirs);

class XDirTask : public XComTask
{
public:
    // 接收到消息的回调
    virtual bool ReadCB(const XMsg *msg);
    // 连接成功的消息回调
    virtual void ConnectedCB();

    // 获取目录后调用的回调
    DirCBFunc DirCB = 0;

    void set_server_root(std::string root) { this->server_root_ = root; }

private:
    // 服务器的目录
    std::string server_root_ = "";
};
