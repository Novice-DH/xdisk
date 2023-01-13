#include "xdisk_client.h"
#include "xcom_task.h"
#include "xthread_pool.h"
#include <iostream>

using namespace std;

bool XDiskClient::Init()
{
    XThreadPool::Get()->Init(10);
    return true;
}

/**
 * @brief 获取目录下的文件列表，只是发送请求消息到服务端
 */
void XDiskClient::GetDir()
{
    cout << "GetDir " << server_ip_ << ":" << server_port_ << "/" << server_root_ << endl;
    auto task = new XComTask();
    task->set_server_ip(server_ip_);
    task->set_port(server_port_);

    // 连接不在此处做，避免阻塞在 GetDir
    XThreadPool::Get()->Dispatch(task);

    // 此时不能操作，task 未初始化，task 没有 event_base
}