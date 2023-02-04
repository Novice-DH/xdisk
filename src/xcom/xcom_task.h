#ifndef XCOM_TASK_H
#define XCOM_TASK_H

#include "xtask.h"
#include "xmsg.h"
#include <string>

class XCOM_API XComTask : public XTask
{
public:
    // 初始化 bufferevent，客户端建立连接
    virtual bool Init();

    virtual void Close();

    void set_server_ip(std::string ip) { this->server_ip_ = ip; }
    void set_port(int port) { this->server_port_ = port; }

    // 事件回调函数
    virtual void EventCB(short what);

    // 读取数据回调函数
    virtual void ReadCB();

    // 接收到消息的回调，由业务类重写
    virtual void ReadCB(const XMsg *msg);

    // 当关闭消息接收时，数据将发送到此函数，由业务模块重写
    virtual void ReadCB(void *data, int size) {}

    // 激活写入回调
    virtual void BeginWrite();

    // 发送消息数据包
    virtual bool Write(const XMsg *msg);
    virtual bool Write(const void *data, int size);

    // 写入数据回调函数
    virtual void WriteCB() {}

    // 连接成功的消息回调，由业务类重写
    virtual void ConnectedCB() {}

    void set_is_recv_msg(bool isrecv) { this->is_recv_msg_ = isrecv; }

    // 子类亦可访问
protected:
    // 读取缓冲
    char read_buf_[4096] = {0};

private:
    // 服务器 IP
    std::string server_ip_ = "";

    // 服务器端口
    int server_port_ = 0;

    struct bufferevent *bev_ = 0;

    // 需要用到 XMsg 对象，故 需要 XCOM_API，若是 指针，则不用导出
    // 数据包缓存
    XMsg msg_;

    // 接收消息开关
    // 接收消息 则调用 void ReadCB(const XMsg *msg)
    // 不接收消息 调用 void ReadCB(void *data, int size)
    bool is_recv_msg_ = true;
};

#endif
