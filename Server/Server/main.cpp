#include <iostream>
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")

DWORD WINAPI thread_func(LPVOID lpParameter)
{
		SOCKET client_scoket = *(SOCKET*)lpParameter;
		free(lpParameter);
	while (1)
	{
		char buffer[1024] = { 0 };
		int ret = recv(client_scoket, buffer, sizeof(buffer), 0);
		if (ret <=0 ) {
			break;
		}
		std::cout << (int)client_scoket << ":" << buffer << std::endl;
		char success[] = "Sent Successfully";
		send(client_scoket, success, sizeof(success),0);
	}
	std::cout << "Socket:" << (int)client_scoket << "disconnect" << std::endl;
	closesocket(client_scoket);
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
		std::cout << "Create Socket Failed! Errcode:" << GetLastError() << std::endl;
		return -1;
	}
	//分配端口号
	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons(8080);
	local.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(listen_scoket, (const sockaddr*)&local, sizeof(local))==-1) {
		std::cout << "Bind Sokcet Failed! Errcode:" << GetLastError() << std::endl;
		return -1;
	}
	//开始监听
	if (listen(listen_scoket, 10) == -1) {
		std::cout << "Listen Scoket Failed! Errcode:" << GetLastError() << std::endl;
		return -1;
	}
	//等待客户端连接
	while (1)
	{
		SOCKET client_scoket = accept(listen_scoket,NULL,0);
		if (client_scoket == INVALID_SOCKET)
			continue;
		std::cout << "New Scoket Connect:" << (int)client_scoket << std::endl;
		SOCKET* socketfd = new SOCKET;
		*socketfd = client_scoket;
		CreateThread(NULL, 0, thread_func, socketfd, NULL, 0);
	}
}