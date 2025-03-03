#include "file_handler.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
namespace fs = std::filesystem;

std::vector<char> ReadFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file) {
        throw std::runtime_error("�޷����ļ���" + filename);
    }

    std::streamsize filesize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(filesize);
    if (!file.read(buffer.data(), filesize)) {
        throw std::runtime_error("�޷���ȡ�ļ���" + filename);
    }

    return buffer;
}

void SendFile(SOCKET client_socket, const std::string& filepath) {
    // �����ļ�·��
    send(client_socket, filepath.c_str(), filepath.size() + 1, 0);

    // ��ȡ�ļ�����
    std::vector<char> fileContent = ReadFile(filepath);

    // �����ļ���С
    uint64_t fileSize = fileContent.size();
    send(client_socket, reinterpret_cast<char*>(&fileSize), sizeof(fileSize), 0);

    // �����ļ�����
    send(client_socket, fileContent.data(), fileContent.size(), 0);
}

void ReceiveFile(SOCKET client_socket, const std::string& savepath) {
    // �����ļ���С
    uint64_t filesize;
    int ret = recv(client_socket, reinterpret_cast<char*>(&filesize), sizeof(filesize), 0);
    if (ret <= 0) {
        throw std::runtime_error("�޷������ļ���С");
    }

    // ����Ŀ¼����������ڣ�
    fs::path path(savepath);
    std::string dir = path.parent_path().string();
    if (!fs::exists(dir)) {
        fs::create_directories(dir);
        std::cout << "�Ѵ���Ŀ¼��" << dir << std::endl;
    }

    // �����ļ�����
    std::ofstream file(savepath, std::ios::binary | std::ios::out);
    if (!file) {
        throw std::runtime_error("�޷������ļ���" + savepath);
    }

    char buffer[1024];
    uint64_t receivedsize = 0;
    while (receivedsize < filesize) {
        ret = recv(client_socket, buffer, sizeof(buffer), 0);
        if (ret <= 0) {
            throw std::runtime_error("�޷������ļ�����");
        }
        file.write(buffer, ret);
        receivedsize += ret;
    }

    std::cout << "�ļ����سɹ���" << savepath << std::endl;
}