#include "file_handler.h"
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <fstream>

using namespace std;
namespace fs = std::filesystem;
// 修复文件路径并提取文件名
std::string FixFilePathAndExtractFileName(const std::string& filepath, const std::string& serverDir) {
    std::string fixedFilePath = filepath;
    std::replace(fixedFilePath.begin(), fixedFilePath.end(), '\\', '/'); // 替换反斜杠为正斜杠

    size_t lastSlash = fixedFilePath.find_last_of('/'); // 找到最后一个斜杠
    std::string fileName = (lastSlash == std::string::npos) ? fixedFilePath : fixedFilePath.substr(lastSlash + 1);

    return serverDir + fileName;
}

// 接收文件大小
uint64_t ReceiveFileSize(SOCKET client_socket) {
    uint64_t fileSize;
    int ret = recv(client_socket, reinterpret_cast<char*>(&fileSize), sizeof(fileSize), 0);
    if (ret <= 0) {
        throw std::runtime_error("Failed to receive file size.");
    }
    return fileSize;
}

// 接收文件内容并保存
void ReceiveAndSaveFile(SOCKET client_socket, const std::string& filepath, uint64_t fileSize) {
    std::ofstream file(filepath, std::ios::binary | std::ios::out);
    if (!file) {
        throw std::runtime_error("Failed to create file: " + filepath);
    }

    char buffer[1024];
    uint64_t receivedSize = 0;
    while (receivedSize < fileSize) {
        int ret = recv(client_socket, buffer, sizeof(buffer), 0);
        if (ret <= 0) {
            throw std::runtime_error("Failed to receive file content.");
        }
        file.write(buffer, ret);
        receivedSize += ret;
    }

    std::cout << "File received: " << filepath << std::endl;
}

void Upload(SOCKET client_socket, const char* filename) {
    std::string serverDir = "D:/Server/";

    // 修复文件路径并提取文件名
    std::string serverFilePath = FixFilePathAndExtractFileName(filename, serverDir);
    std::cout << "Server file path: " << serverFilePath << std::endl;

    // 接收文件大小
    uint64_t fileSize = ReceiveFileSize(client_socket);

    // 接收文件内容并保存
    ReceiveAndSaveFile(client_socket, serverFilePath, fileSize);
}

// 获取文件列表
std::string GetFileList(const std::string& path) {
    std::string filelist;
    for (const auto& entry : fs::directory_iterator(path)) {
        filelist += entry.path().filename().string() + "\n";
    }
    return filelist;
}

// 发送文件列表
void SendFileList(SOCKET client_socket, const std::string& filelist) {
    uint64_t filelistSize = filelist.size();
    send(client_socket, reinterpret_cast<char*>(&filelistSize), sizeof(filelistSize), 0);
    send(client_socket, filelist.c_str(), filelistSize, 0);
    std::cout << "Sent file list: " << filelist << std::endl;
}

// 接收文件名
std::string ReceiveFileName(SOCKET client_socket) {
    uint64_t filenamesize;
    int ret = recv(client_socket, reinterpret_cast<char*>(&filenamesize), sizeof(filenamesize), 0);
    if (ret <= 0) {
        throw std::runtime_error("Failed to receive filename size.");
    }

    std::vector<char> filenameBuffer(filenamesize + 1);
    uint64_t receivedsize = 0;
    while (receivedsize < filenamesize) {
        ret = recv(client_socket, filenameBuffer.data() + receivedsize, filenamesize - receivedsize, 0);
        if (ret <= 0) {
            throw std::runtime_error("Failed to receive filename.");
        }
        receivedsize += ret;
    }
    filenameBuffer[filenamesize] = '\0';
    return std::string(filenameBuffer.data());
}

// 发送文件内容
void SendFileContent(SOCKET client_socket, const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (!file) {
        throw std::runtime_error("Failed to open the file: " + filepath);
    }

    std::streamsize filesize = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<char> filecontent(filesize);
    if (!file.read(filecontent.data(), filesize)) {
        throw std::runtime_error("Failed to read the file: " + filepath);
    }

    send(client_socket, reinterpret_cast<char*>(&filesize), sizeof(filesize), 0);
    send(client_socket, filecontent.data(), filesize, 0);
    std::cout << "File sent successfully: " << filepath << std::endl;
}

void Download(SOCKET client_socket) {
    std::string path = "D:/Server";

    // 获取并发送文件列表
    std::string filelist = GetFileList(path);
    SendFileList(client_socket, filelist);

    // 接收文件名
    std::string filename = ReceiveFileName(client_socket);

    // 发送文件内容
    std::string filepath = path + "/" + filename;
    SendFileContent(client_socket, filepath);
}