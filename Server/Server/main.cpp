#include <iostream>
#include <WinSock2.h>
#include <fstream>
#include <string>
#include <algorithm>
#include <filesystem>
#pragma comment(lib,"ws2_32.lib")
using namespace std;
namespace fs = std::filesystem;
void Upload(SOCKET client_socket, const char* filename)
{
    //将"\"的不合法路径改成"/"的合法路径，并且提出文件名与服务器指定路径拼接
    string serverDir = "D:/Server/";
    string fixdfile = filename;
    replace(fixdfile.begin(), fixdfile.end(), '\\', '/'); // 替换反斜杠为正斜杠
    size_t lastSlash = fixdfile.find_last_of('/'); // 找到最后一个斜杠
    string fileName = (lastSlash == string::npos) ? fixdfile : fixdfile.substr(lastSlash + 1);
    string serverFilePath = serverDir + fileName;

    cout << "Server file path: " << serverFilePath << endl;
    ofstream file(serverFilePath, ios::binary | ios::out);
    if (!file) {
        throw runtime_error("Failed to create file:" + serverFilePath);
    }

    // 接收文件大小
    uint64_t fileSize;
    recv(client_socket, reinterpret_cast<char*>(&fileSize), sizeof(fileSize), 0);

    // 接收文件内容
    char buffer[1024];
    uint64_t receivedSize = 0;
    while (receivedSize < fileSize) {
        int ret = recv(client_socket, buffer, sizeof(buffer), 0);
        if (ret <= 0) break;
        file.write(buffer, ret);
        receivedSize += ret;
    }

    cout << "File received: " << filename << endl;
}
void Download(SOCKET client_socket)
{
    // 展示文件结构
    string path = "D:/Server";
    string filelist;
    for (const auto& entry : fs::directory_iterator(path))
    {
        filelist += entry.path().filename().string() + "\n";  // 输出文件名
    }

    // 发送文件列表的大小
    uint64_t filelistSize = filelist.size();
    send(client_socket, reinterpret_cast<char*>(&filelistSize), sizeof(filelistSize), 0);

    // 发送文件列表
    send(client_socket, filelist.c_str(), filelistSize, 0);
    cout << "Sent file list: " << filelist << endl;

    // 接收文件名大小
    uint64_t filenamesize;
    int ret = recv(client_socket, reinterpret_cast<char*>(&filenamesize), sizeof(filenamesize), 0);
    if (ret <= 0) {
        cerr << "Failed to receive filename size." << endl;
        return;
    }

    // 接收文件名
    vector<char> filenameBuffer(filenamesize + 1); // 动态分配缓冲区，多一个字节用于终止符
    uint64_t receivedsize = 0;
    while (receivedsize < filenamesize)
    {
        ret = recv(client_socket, filenameBuffer.data() + receivedsize, filenamesize - receivedsize, 0);
        if (ret <= 0) {
            cerr << "Failed to receive filename." << endl;
            return;
        }
        receivedsize += ret;
    }
    filenameBuffer[filenamesize] = '\0'; // 手动添加终止符
    string filename(filenameBuffer.data()); // 将缓冲区数据转换为 std::string

    // 发送文件内容大小
    string filepath = "D:/Server/" + filename;
    ifstream file(filepath, ios::binary | ios::ate);
    if (!file) {
        throw runtime_error("Failed to open the file: " + filepath);
    }

    // 获取文件大小
    streamsize filesize = file.tellg();

    // 移动到文件开头
    file.seekg(0, ios::beg);
    vector<char> filecontent(filesize);
    if (!file.read(filecontent.data(), filesize)) {
        throw runtime_error("Failed to read the file: " + filepath);
    }

    // 发送文件大小
    send(client_socket, reinterpret_cast<char*>(&filesize), sizeof(filesize), 0);

    // 发送文件内容
    send(client_socket, filecontent.data(), filesize, 0);

    cout << "File sent successfully: " << filename << endl;
}
DWORD WINAPI thread_func(LPVOID lpParameter)
{
    SOCKET client_socket = *(SOCKET*)lpParameter;
    free(lpParameter);

    while (1)
    {
        char rbuffer[1024] = { 0 };
        int ret = recv(client_socket, rbuffer, sizeof(rbuffer), 0);
        if (ret <= 0) break;

        if (strcmp(rbuffer, "上传文件") == 0) {
            ret = recv(client_socket, rbuffer, sizeof(rbuffer), 0);
            if (ret <= 0) break;

            Upload(client_socket, rbuffer);
        }
        if (strcmp(rbuffer, "下载文件") == 0) {
            Download(client_socket);
        }

        cout << (int)client_socket << ":" << rbuffer << endl;
    }

    cout << "Socket:" << (int)client_socket << " disconnect" << endl;
    closesocket(client_socket);
    return 0;
}

int main()
{
	//开启网络权限
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	//创建socket套接字
	SOCKET listen_scoket = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_scoket == INVALID_SOCKET) {
		cout << "Create Socket Failed! Errcode:" << GetLastError() << endl;
		return -1;
	}
	//分配端口号
	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons(8080);
	local.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(listen_scoket, (const sockaddr*)&local, sizeof(local))==-1) {
		cout << "Bind Sokcet Failed! Errcode:" << GetLastError() << endl;
		return -1;
	}
	//开始监听
	if (listen(listen_scoket, 10) == -1) {
		cout << "Listen Scoket Failed! Errcode:" << GetLastError() << endl;
		return -1;
	}
	//等待客户端连接
	while (1)
	{
		SOCKET client_scoket = accept(listen_scoket,NULL,0);
		if (client_scoket == INVALID_SOCKET)
			continue;
		cout << "New Scoket Connect:" << (int)client_scoket << endl;
		SOCKET* socketfd = new SOCKET;
		*socketfd = client_scoket;
		CreateThread(NULL, 0, thread_func, socketfd, NULL, 0);
	}
}
