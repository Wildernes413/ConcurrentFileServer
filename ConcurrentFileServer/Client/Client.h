#pragma once
#ifndef CLIENT_H
#define CLIENT_H

#include <WinSock2.h>
#include <string>
#pragma comment(lib,"ws2_32.lib")
class FileClient {
public:
    FileClient();
    ~FileClient();
    bool Connect(const std::string& ip, int port);
    void Run();

private:
    SOCKET client_socket;
    void UploadFile(const std::string& filepath);
    void DownloadFile();
};

#endif // CLIENT_H