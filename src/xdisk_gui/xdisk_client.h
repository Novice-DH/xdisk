#pragma once
#include <string>

class XDiskClient
{
public:
    // 单件模式
    static XDiskClient *Get()
    {
        static XDiskClient xc;
        return &xc;
    }

    // 初始化，包括线程池
    bool Init();

    /**
     * @brief 获取目录下的文件列表，只是发送请求消息到服务端
     */
    void GetDir();

    void set_server_ip(std::string ip) { this->server_ip_ = ip; }
    void set_server_root(std::string root) { this->server_root_ = root; }
    void set_port(int port) { this->server_port_ = port; }

private:
    // 服务器 IP
    std::string server_ip_ = "";

    // 服务器的目录
    std::string server_root_ = "";

    // 服务器端口
    int server_port_ = 0;

    XDiskClient() {};
};
