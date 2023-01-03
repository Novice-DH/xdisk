#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h> // 网络地址转换
#include <unistd.h>    // close
#include <string.h>    // 字符集转换

using namespace std;

int main(int argc, char *argv[])
{
    unsigned short port = 8080;
    if (argc > 1)
    {
        port = atoi(argv[1]);
    }
    // 1 create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock <= 0)
    {
        cerr << "creator socket error " << strerror(errno) << endl;
        return -1; // 一旦出错，立即退出
    }
    // 2 bind port
    sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = htonl(0);
    int re = ::bind(sock, (sockaddr *)&saddr, sizeof(saddr));
    if (re != 0)
    {
        cerr << "bind port " << port << " failed!" << strerror(errno) << endl;
        return -1;
    }
    cout << "bind port " << port << " success!" << endl;
    // 3 listen
    listen(sock, 10);
    // 4 accept
    sockaddr_in caddr;
    // int len = 0; // win 可直接用 int，Linux 不行
    socklen_t addrlen = 0;
    int client_sock = accept(sock, (sockaddr *)&caddr, &addrlen);
    cout << "client sock = " << client_sock << endl;

    // send
    char buf[1024] = "welcome to xms";
    int len = send(client_sock, buf, strlen(buf), 0);
    cout << "send len = " << len << endl;

    // recv
    len = recv(client_sock, buf, sizeof(buf) - 1, 0); // 留一位给 换行符
    if (len > 0)
    {
        buf[len] = '\0';
        cout << buf << endl;
    }

    close(sock);
    return 0;
}