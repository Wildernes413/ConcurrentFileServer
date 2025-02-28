#include <iostream>
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
int main()
{
	//��������Ȩ��
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	//����socket�׽���
	SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == INVALID_SOCKET) {
		std::cout << "Create Socket Failed! Errcode:" << GetLastError() << std::endl;
		return -1;
	}
	//���ӷ�����
	struct sockaddr_in target;
	target.sin_family = AF_INET;
	target.sin_port = htons(8080);
	target.sin_addr.s_addr = inet_addr("127.0.0.1");//�������޸�Ŀ���������ַ

	if (connect(client_socket, (const sockaddr*)&target, sizeof(target)) == -1) {
		std::cout << "Connect Socket Failed! Errcode:" << GetLastError() << std::endl;
		closesocket(client_socket);
		return -1;
	}
	//��ʼͨѶ
	while (1)
	{
		char sbuffer[1024] = { 0 };
		std::cout << "������" << std::endl;
		std::cin >> sbuffer;
		send(client_socket, sbuffer, sizeof(sbuffer), 0);
		char rbuffer[1024] = { 0 };
		int ret=recv(client_socket, rbuffer, sizeof(rbuffer), 0);
		if (ret <= 0)
			break;
		std::cout << rbuffer << std::endl;
	}
	closesocket(client_socket);
	return 0;
}