#include <iostream>
#include <WinSock2.h>
#include <fstream>
#include <vector>
#include <algorithm>
#include <filesystem>
#pragma comment(lib,"ws2_32.lib")
using namespace std;
vector<char> readfile(const string& filename)
{
	// 以二进制打开文件，并移动到文件末尾
	ifstream file(filename, ios::ate | ios::binary);
	if (!file) {
		throw runtime_error("Failed to open the file:" + filename);
	}
	// 获取文件大小
	streamsize filesize = file.tellg();
	// 移动到文件开头
	file.seekg(0, ios::beg);
	vector<char> buffer(filesize);
	if (!file.read(buffer.data(), filesize)) {
		throw runtime_error("Failed to read the file:" + filename);
	}
	return buffer;
}

int main()
{
	//开启网络权限
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	//创建socket套接字
	SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == INVALID_SOCKET) {
		cout << "创建套接字失败！错误代码：" << GetLastError() << endl;
		return -1;
	}
	//连接服务器
	struct sockaddr_in target;
	target.sin_family = AF_INET;
	target.sin_port = htons(8080);
	target.sin_addr.s_addr = inet_addr("26.254.219.0");//在这里修改目标服务器地址

	if (connect(client_socket, (const sockaddr*)&target, sizeof(target)) == -1) {
		cout << "连接套接字失败！错误代码：" << GetLastError() << endl;
		closesocket(client_socket);
		return -1;
	}
	//开始通讯

	while (1)
	{
		char sbuffer[1024] = { 0 };
		cout << "请选择您要执行的操作：" << endl << "上传文件" << endl << "下载文件" << endl;
		cin >> sbuffer;

		char rbuffer[1024] = { 0 };
		if (strcmp(sbuffer, "上传文件") == 0) {
			send(client_socket, sbuffer, sizeof(sbuffer), 0);
			cout << "请输入您要上传的文件地址" << endl;
			cin >> sbuffer; // sbuffer：要上传的文件地址

			// 发送文件路径
			send(client_socket, sbuffer, sizeof(sbuffer), 0);

			// 读取文件内容
			vector<char> fileContent = readfile(sbuffer);

			// 发送文件大小
			uint64_t fileSize = fileContent.size();
			send(client_socket, reinterpret_cast<char*>(&fileSize), sizeof(fileSize), 0);

			// 发送文件内容
			send(client_socket, fileContent.data(), fileContent.size(), 0);
		}
        else if (strcmp(sbuffer, "下载文件") == 0) {
            send(client_socket, sbuffer, sizeof(sbuffer), 0);
            cout << "请选择您要下载的文件" << endl;

            // 接收文件列表大小
            uint64_t filelistsize = 0;
            int ret = recv(client_socket, reinterpret_cast<char*>(&filelistsize), sizeof(filelistsize), 0);
            if (ret <= 0) {
                cerr << "Failed to receive file list size." << endl;
                return 0; 
            }

            // 接收文件列表
            uint64_t receivedlistsize = 0;
            char rbuffer[1024] = { 0 };
            while (receivedlistsize < filelistsize) {
                ret = recv(client_socket, rbuffer + receivedlistsize, sizeof(rbuffer) - receivedlistsize, 0);
                if (ret <= 0) {
                    cerr << "Failed to receive file list." << endl;
                    return 0;
                }
                receivedlistsize += ret;
            }
            rbuffer[filelistsize] = '\0'; // 手动添加终止符
            cout << "Available files:\n" << rbuffer << endl;

            // 用户选择文件
            string filename;
            cin >> filename;

            // 发送文件名大小
            uint64_t filenamesize = filename.size();
            send(client_socket, reinterpret_cast<char*>(&filenamesize), sizeof(filenamesize), 0);

            // 发送文件名
            send(client_socket, filename.c_str(), filenamesize, 0);

            // 接收文件内容大小
            uint64_t filesize;
            ret = recv(client_socket, reinterpret_cast<char*>(&filesize), sizeof(filesize), 0);
            if (ret <= 0) {
                cerr << "Failed to receive file size." << endl;
                return 0;
            }

            // 创建目录（如果不存在）
            string filepath = "D:/Client/" + filename;
            filesystem::path path(filepath);
            string dir = path.parent_path().string();
            if (!filesystem::exists(dir)) {
                filesystem::create_directories(dir); // 创建目录
                cout << "Created directory: " << dir << endl;
            }

            // 接收文件内容
            ofstream file(filepath, ios::binary | ios::out);
            if (!file) {
                cerr << "Failed to create file: " << filepath << endl;
                return 0;
            }

            uint64_t receivedsize = 0;
            while (receivedsize < filesize) {
                ret = recv(client_socket, rbuffer, sizeof(rbuffer), 0);
                if (ret <= 0) {
                    cerr << "Failed to receive file content." << endl;
                    break;
                }
                file.write(rbuffer, ret);
                receivedsize += ret;
            }

            cout << "File downloaded successfully: " << filepath << endl;
        }
		else {
			cout << "无效的输入，请输入“上传”或“下载”以继续。" << endl;
		}
	}
	closesocket(client_socket);
	return 0;
}
