﻿#include <event2/event.h>
#include <iostream>

using namespace std;

int main()
{
#ifdef _WIN32
    //初始化 socket 库
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
     
    std::cout << "test libevent!\n";
    // 创建 libevent 的上下文
    event_base * base = event_base_new();
    if (base) {
        cout << "event_base_new success!" << endl;
    }
    return 0;
}
