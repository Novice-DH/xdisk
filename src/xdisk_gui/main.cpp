#ifdef _WIN32
#include <windows.h>
#else
#include <signal.h>
#endif

#include "xdisk_gui.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
#ifdef _WIN32
    // 初始化 socket 库
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
#else
    // Linux 使用断开连接的 socket 会发出此信号，造成程序退出
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
    {
        return 1;
    }
#endif

    QApplication a(argc, argv);
    XDiskGUI w;
    w.show();
    // 处理信号槽
    return a.exec();
}
