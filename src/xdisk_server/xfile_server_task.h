#pragma once
#include "xcom_task.h"
#include <fstream>
#include <mutex>

class XFileServerTask : public XComTask
{
public:
    // 接收到消息的回调
    virtual void ReadCB(const XMsg *msg);

    // 当关闭消息接收时，数据将发送到此函数，由业务模块重写
    virtual void ReadCB(void *data, int size);

    static void set_cur_dir(std::string dir)
    {
        cur_dir_mux_.lock();
        cur_dir_ = dir;
        cur_dir_mux_.unlock();
    }
    static std::string cur_dir()
    {
        cur_dir_mux_.lock();
        std::string dir = cur_dir_;
        cur_dir_mux_.unlock();
        return dir;
    }

private:
    // 处理目录获取的消息，返回目录列表
    void GetDir(const XMsg *msg);

    // 处理客户端的上传请求
    void Upload(const XMsg *msg);

    // 文件大小
    int filesize_ = 0;

    // 客户已上传的文件大小
    int upload_size_ = 0;

    // 当前路径
    // std::string cur_dir_ = "./";
    static std::string cur_dir_;
    static std::mutex cur_dir_mux_;

    // 写入上传的文件
    std::ofstream ofs_;
};
