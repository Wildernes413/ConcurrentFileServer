#include "server.h"
#include "file_handler.h"
#include <iostream>
#include "user.h"
FileServer::FileServer() : listen_socket(INVALID_SOCKET) {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
}

FileServer::~FileServer() {
    if (listen_socket != INVALID_SOCKET) {
        closesocket(listen_socket);
    }
    WSACleanup();
}

void FileServer::Start(int port) {
    listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socket == INVALID_SOCKET) {
        std::cerr << "Create Socket Failed! Errcode:" << GetLastError() << std::endl;
        return;
    }

    sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = htons(port);
    local.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listen_socket, (const sockaddr*)&local, sizeof(local)) == -1) {
        std::cerr << "Bind Socket Failed! Errcode:" << GetLastError() << std::endl;
        return;
    }

    if (listen(listen_socket, 10) == -1) {
        std::cerr << "Listen Socket Failed! Errcode:" << GetLastError() << std::endl;
        return;
    }

    std::cout << "Server started on port " << port << std::endl;

    while (true) {
        SOCKET client_socket = accept(listen_socket, NULL, 0);
        if (client_socket == INVALID_SOCKET) continue;

        std::cout << "New Socket Connect:" << (int)client_socket << std::endl;
        SOCKET* socketfd = new SOCKET;
        *socketfd = client_socket;
        CreateThread(NULL, 0, ThreadFunc, socketfd, 0, NULL);
    }
}

DWORD WINAPI FileServer::ThreadFunc(LPVOID lpParameter) {
    SOCKET client_socket = *(SOCKET*)lpParameter;
    delete (SOCKET*)lpParameter;
    User::LoginAndRegister(client_socket);
    char buffer[1024] = { 0 };
    while (true) {
        int ret = recv(client_socket, buffer, sizeof(buffer), 0);
        if (ret <= 0) break;

        if (strcmp(buffer, "上传文件") == 0) {
            ret = recv(client_socket, buffer, sizeof(buffer), 0);
            if (ret <= 0) break;
            Upload(client_socket, buffer);
        }
        else if (strcmp(buffer, "下载文件") == 0) {
            Download(client_socket);
        }
    }

    std::cout << "Socket:" << (int)client_socket << " disconnect" << std::endl;
    closesocket(client_socket);
    return 0;
}