#include "file_handler.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
namespace fs = std::filesystem;

std::vector<char> ReadFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file) {
        throw std::runtime_error("无法打开文件：" + filename);
    }

    std::streamsize filesize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(filesize);
    if (!file.read(buffer.data(), filesize)) {
        throw std::runtime_error("无法读取文件：" + filename);
    }

    return buffer;
}

void SendFile(SOCKET client_socket, const std::string& filepath) {
    // 发送文件路径
    send(client_socket, filepath.c_str(), filepath.size() + 1, 0);

    // 读取文件内容
    std::vector<char> fileContent = ReadFile(filepath);

    // 发送文件大小
    uint64_t fileSize = fileContent.size();
    send(client_socket, reinterpret_cast<char*>(&fileSize), sizeof(fileSize), 0);

    // 发送文件内容
    send(client_socket, fileContent.data(), fileContent.size(), 0);
}

void ReceiveFile(SOCKET client_socket, const std::string& savepath) {
    // 接收文件大小
    uint64_t filesize;
    int ret = recv(client_socket, reinterpret_cast<char*>(&filesize), sizeof(filesize), 0);
    if (ret <= 0) {
        throw std::runtime_error("无法接收文件大小");
    }

    // 创建目录（如果不存在）
    fs::path path(savepath);
    std::string dir = path.parent_path().string();
    if (!fs::exists(dir)) {
        fs::create_directories(dir);
        std::cout << "已创建目录：" << dir << std::endl;
    }

    // 接收文件内容
    std::ofstream file(savepath, std::ios::binary | std::ios::out);
    if (!file) {
        throw std::runtime_error("无法创建文件：" + savepath);
    }

    char buffer[1024];
    uint64_t receivedsize = 0;
    while (receivedsize < filesize) {
        ret = recv(client_socket, buffer, sizeof(buffer), 0);
        if (ret <= 0) {
            throw std::runtime_error("无法接收文件内容");
        }
        file.write(buffer, ret);
        receivedsize += ret;
    }

    std::cout << "文件下载成功：" << savepath << std::endl;
}