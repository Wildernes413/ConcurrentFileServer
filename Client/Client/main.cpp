#include <iostream>
#include <WinSock2.h>
#include <fstream>
#include <vector>
#pragma comment(lib,"ws2_32.lib")
using namespace std;
int main()
{
	//开启网络权限
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	//创建socket套接字
	SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == INVALID_SOCKET) {
		cout << "Create Socket Failed! Errcode:" << GetLastError() << endl;
		return -1;
	}
	//连接服务器
	struct sockaddr_in target;
	target.sin_family = AF_INET;
	target.sin_port = htons(8080);
	target.sin_addr.s_addr = inet_addr("127.0.0.1");//在这里修改目标服务器地址

	if (connect(client_socket, (const sockaddr*)&target, sizeof(target)) == -1) {
		cout << "Connect Socket Failed! Errcode:" << GetLastError() << endl;
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
			cin >> sbuffer;												//sbuffer：要上传的文件地址
			Upload(sbuffer);
			send(client_socket, sbuffer, sizeof(sbuffer), 0);
		}
		else if (strcmp(sbuffer, "下载文件") == 0) {
			send(client_socket, sbuffer, sizeof(sbuffer), 0);
			cout << "请选择您要下载的文件" << endl;			
			int ret = recv(client_socket, rbuffer, sizeof(rbuffer), 0);			//rbuffer:可供下载的文件
			if (ret <= 0) break;
			cout << rbuffer << std::endl;
			cin >> sbuffer;												//sbuffer:要下载的文件
			int ret = recv(client_socket, rbuffer, sizeof(rbuffer), 0);
			if (ret <= 0) break;												//rbuffer:文件
		}
		else {
			cout << "无效的输入，请输入“上传”或“下载”以继续。" << endl;
		}
	}
	closesocket(client_socket);
	return 0;
}
vector<char> Upload(const string filename)
{
	//以二进制打开文件，并移动到文件末尾
	ifstream file(filename, ios::ate | ios::binary);
	if (!file) {
		throw runtime_error("Failed to open the file:" + filename);
	}
	//获取文件大小
	streamsize filesize = file.tellg();
	//移动到文件开头
	file.seekg(0, ios::beg);
	vector<char>buffer(filesize);
	if (!file.read(buffer.data(), filesize)) {
		throw runtime_error("Failed to read the file:" + filename);
	}
	return buffer;
}