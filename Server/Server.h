#pragma once
#ifndef SERVER_H
#define SERVER_H

#include <WinSock2.h>
#include <string>
#pragma comment(lib,"ws2_32.lib")
class FileServer {
public:
    FileServer();
    ~FileServer();
    void Start(int port);

private:
    SOCKET listen_socket;
    static DWORD WINAPI ThreadFunc(LPVOID lpParameter);
};

#endif // SERVER_H