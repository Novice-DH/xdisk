#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <sys/types.h>
#if _WIN32
#include <windows.h>
#define close closesocket
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif
#include <string.h>
#pragma comment(lib, "ws2_32.lib") // 加载 ws2_32.lib
using namespace std;

int main(int argc, char *argv[])
{
    // Windows初始化socket库
#ifdef _WIN32
    WSADATA ws;
    WSAStartup(MAKEWORD(2, 2), &ws);
#endif
    unsigned short port = 8080;
    const char *ip = "127.0.0.1";
    // tcpclient 172.20.176.5 8080
    if (argc > 2)
    {
        ip = argv[1];
        port = atoi(argv[2]);
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        cerr << "creator socket failed!" << strerror(errno) << endl;
        return -1;
    }
    sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = inet_addr(ip);

    int re = connect(sock, (sockaddr *)&saddr, sizeof(saddr));
    if (re != 0)
    {
        cerr << "connect " << ip << ":" << port << " failed!" << strerror(errno) << endl;
        return -1;
    }
    cout << "connect " << ip << ":" << port << " success!" << endl;

    char buf[1024] = {0};
    int len = recv(sock, buf, sizeof(buf) - 1, 0);
    if (len > 0)
    {
        cout << buf << endl; // 由于初始化为全0，故末尾无需加 \0
    }

    strcpy(buf, "send from client");
    len = send(sock, buf, strlen(buf), 0);
    close(sock);

    return 0;
}