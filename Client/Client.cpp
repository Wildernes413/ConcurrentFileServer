#include "client.h"
#include "file_handler.h"
#include <iostream>

FileClient::FileClient() : client_socket(INVALID_SOCKET) {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
}

FileClient::~FileClient() {
    if (client_socket != INVALID_SOCKET) {
        closesocket(client_socket);
    }
    WSACleanup();
}

bool FileClient::Connect(const std::string& ip, int port) {
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        std::cerr << "创建套接字失败！错误代码：" << GetLastError() << std::endl;
        return false;
    }

    sockaddr_in target;
    target.sin_family = AF_INET;
    target.sin_port = htons(port);
    target.sin_addr.s_addr = inet_addr(ip.c_str());

    if (connect(client_socket, (const sockaddr*)&target, sizeof(target)) == -1) {
        std::cerr << "连接服务器失败！错误代码：" << GetLastError() << std::endl;
        closesocket(client_socket);
        return false;
    }

    std::cout << "成功连接到服务器：" << ip << ":" << port << std::endl;
    return true;
}

void FileClient::Run() {
    while (true) {
        char sbuffer[1024] = { 0 };
        std::cout << "请选择您要执行的操作：" << std::endl
            << "上传文件" << std::endl
            << "下载文件" << std::endl;
        std::cin >> sbuffer;
        send(client_socket, sbuffer, sizeof(sbuffer), 0);
        if (strcmp(sbuffer, "上传文件") == 0) {
            std::string filepath;          
            std::cout << "请输入您要上传的文件地址：" << std::endl;
            std::cin >> filepath;
           // UploadFile(filepath);
            SendFile(FileClient::client_socket, filepath);
        }
        else if (strcmp(sbuffer, "下载文件") == 0) {
            //DownloadFile();
            ReceiveFile(FileClient::client_socket, "D:/Client/");
        }
        else {
            std::cout << "无效的输入，请输入“上传文件”或“下载文件”以继续。" << std::endl;
        }
    }
}