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
        std::cerr << "�����׽���ʧ�ܣ�������룺" << GetLastError() << std::endl;
        return false;
    }

    sockaddr_in target;
    target.sin_family = AF_INET;
    target.sin_port = htons(port);
    target.sin_addr.s_addr = inet_addr(ip.c_str());

    if (connect(client_socket, (const sockaddr*)&target, sizeof(target)) == -1) {
        std::cerr << "���ӷ�����ʧ�ܣ�������룺" << GetLastError() << std::endl;
        closesocket(client_socket);
        return false;
    }

    std::cout << "�ɹ����ӵ���������" << ip << ":" << port << std::endl;
    return true;
}

void FileClient::Run() {
    while (true) {
        char sbuffer[1024] = { 0 };
        std::cout << "��ѡ����Ҫִ�еĲ�����" << std::endl
            << "�ϴ��ļ�" << std::endl
            << "�����ļ�" << std::endl;
        std::cin >> sbuffer;
        send(client_socket, sbuffer, sizeof(sbuffer), 0);
        if (strcmp(sbuffer, "�ϴ��ļ�") == 0) {
            std::string filepath;          
            std::cout << "��������Ҫ�ϴ����ļ���ַ��" << std::endl;
            std::cin >> filepath;
           // UploadFile(filepath);
            SendFile(FileClient::client_socket, filepath);
        }
        else if (strcmp(sbuffer, "�����ļ�") == 0) {
            //DownloadFile();
            ReceiveFile(FileClient::client_socket, "D:/Client/");
        }
        else {
            std::cout << "��Ч�����룬�����롰�ϴ��ļ����������ļ����Լ�����" << std::endl;
        }
    }
}