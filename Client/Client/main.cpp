#include <iostream>
#include <WinSock2.h>
#include <fstream>
#include <vector>
#pragma comment(lib,"ws2_32.lib")
using namespace std;
int main()
{
	//��������Ȩ��
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	//����socket�׽���
	SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == INVALID_SOCKET) {
		cout << "Create Socket Failed! Errcode:" << GetLastError() << endl;
		return -1;
	}
	//���ӷ�����
	struct sockaddr_in target;
	target.sin_family = AF_INET;
	target.sin_port = htons(8080);
	target.sin_addr.s_addr = inet_addr("127.0.0.1");//�������޸�Ŀ���������ַ

	if (connect(client_socket, (const sockaddr*)&target, sizeof(target)) == -1) {
		cout << "Connect Socket Failed! Errcode:" << GetLastError() << endl;
		closesocket(client_socket);
		return -1;
	}
	//��ʼͨѶ

	while (1)
	{
		char sbuffer[1024] = { 0 };
		cout << "��ѡ����Ҫִ�еĲ�����" << endl << "�ϴ��ļ�" << endl << "�����ļ�" << endl;
		cin >> sbuffer;

		char rbuffer[1024] = { 0 };
		if (strcmp(sbuffer, "�ϴ��ļ�") == 0) {							
			send(client_socket, sbuffer, sizeof(sbuffer), 0);
			cout << "��������Ҫ�ϴ����ļ���ַ" << endl;
			cin >> sbuffer;												//sbuffer��Ҫ�ϴ����ļ���ַ
			Upload(sbuffer);
			send(client_socket, sbuffer, sizeof(sbuffer), 0);
		}
		else if (strcmp(sbuffer, "�����ļ�") == 0) {
			send(client_socket, sbuffer, sizeof(sbuffer), 0);
			cout << "��ѡ����Ҫ���ص��ļ�" << endl;			
			int ret = recv(client_socket, rbuffer, sizeof(rbuffer), 0);			//rbuffer:�ɹ����ص��ļ�
			if (ret <= 0) break;
			cout << rbuffer << std::endl;
			cin >> sbuffer;												//sbuffer:Ҫ���ص��ļ�
			int ret = recv(client_socket, rbuffer, sizeof(rbuffer), 0);
			if (ret <= 0) break;												//rbuffer:�ļ�
		}
		else {
			cout << "��Ч�����룬�����롰�ϴ��������ء��Լ�����" << endl;
		}
	}
	closesocket(client_socket);
	return 0;
}
vector<char> Upload(const string filename)
{
	//�Զ����ƴ��ļ������ƶ����ļ�ĩβ
	ifstream file(filename, ios::ate | ios::binary);
	if (!file) {
		throw runtime_error("Failed to open the file:" + filename);
	}
	//��ȡ�ļ���С
	streamsize filesize = file.tellg();
	//�ƶ����ļ���ͷ
	file.seekg(0, ios::beg);
	vector<char>buffer(filesize);
	if (!file.read(buffer.data(), filesize)) {
		throw runtime_error("Failed to read the file:" + filename);
	}
	return buffer;
}