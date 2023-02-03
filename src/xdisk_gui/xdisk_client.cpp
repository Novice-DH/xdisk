#include "xdisk_client.h"
#include "xdir_task.h"
#include "xupload_task.h"
#include "xthread_pool.h"
#include <iostream>
#include <string>

using namespace std;

bool XDiskClient::Init()
{
    XThreadPool::Get()->Init(10);
    return true;
}

static void DirCB(string dirs)
{
    cout << dirs << endl;
    XDiskClient::Get()->SDir(dirs);
}

/**
 * @brief 获取目录下的文件列表，只是发送请求消息到服务端
 */
void XDiskClient::GetDir()
{
    cout << "GetDir " << server_ip_ << ":" << server_port_ << "/" << server_root_ << endl;
    auto task = new XDirTask();
    task->set_server_ip(server_ip_);
    task->set_port(server_port_);
    task->set_server_root(server_root_);
    task->DirCB = DirCB;

    // 连接不在此处做，避免阻塞在 GetDir
    XThreadPool::Get()->Dispatch(task);

    // 此时不能操作，task 未初始化，task 没有 event_base
}

/**
 * @brief 上传文件请求
 *
 * @param filepath 本地文件路径
 */
void XDiskClient::Upload(std::string filepath)
{
    auto task = new XUploadTask();
    task->set_server_ip(server_ip_);
    task->set_port(server_port_);
    task->set_filepath(filepath);
    XThreadPool::Get()->Dispatch(task);
}
