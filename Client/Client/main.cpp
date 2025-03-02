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
	// �Զ����ƴ��ļ������ƶ����ļ�ĩβ
	ifstream file(filename, ios::ate | ios::binary);
	if (!file) {
		throw runtime_error("Failed to open the file:" + filename);
	}
	// ��ȡ�ļ���С
	streamsize filesize = file.tellg();
	// �ƶ����ļ���ͷ
	file.seekg(0, ios::beg);
	vector<char> buffer(filesize);
	if (!file.read(buffer.data(), filesize)) {
		throw runtime_error("Failed to read the file:" + filename);
	}
	return buffer;
}

int main()
{
	//��������Ȩ��
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	//����socket�׽���
	SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == INVALID_SOCKET) {
		cout << "�����׽���ʧ�ܣ�������룺" << GetLastError() << endl;
		return -1;
	}
	//���ӷ�����
	struct sockaddr_in target;
	target.sin_family = AF_INET;
	target.sin_port = htons(8080);
	target.sin_addr.s_addr = inet_addr("26.254.219.0");//�������޸�Ŀ���������ַ

	if (connect(client_socket, (const sockaddr*)&target, sizeof(target)) == -1) {
		cout << "�����׽���ʧ�ܣ�������룺" << GetLastError() << endl;
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
			cin >> sbuffer; // sbuffer��Ҫ�ϴ����ļ���ַ

			// �����ļ�·��
			send(client_socket, sbuffer, sizeof(sbuffer), 0);

			// ��ȡ�ļ�����
			vector<char> fileContent = readfile(sbuffer);

			// �����ļ���С
			uint64_t fileSize = fileContent.size();
			send(client_socket, reinterpret_cast<char*>(&fileSize), sizeof(fileSize), 0);

			// �����ļ�����
			send(client_socket, fileContent.data(), fileContent.size(), 0);
		}
        else if (strcmp(sbuffer, "�����ļ�") == 0) {
            send(client_socket, sbuffer, sizeof(sbuffer), 0);
            cout << "��ѡ����Ҫ���ص��ļ�" << endl;

            // �����ļ��б��С
            uint64_t filelistsize = 0;
            int ret = recv(client_socket, reinterpret_cast<char*>(&filelistsize), sizeof(filelistsize), 0);
            if (ret <= 0) {
                cerr << "Failed to receive file list size." << endl;
                return 0; 
            }

            // �����ļ��б�
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
            rbuffer[filelistsize] = '\0'; // �ֶ������ֹ��
            cout << "Available files:\n" << rbuffer << endl;

            // �û�ѡ���ļ�
            string filename;
            cin >> filename;

            // �����ļ�����С
            uint64_t filenamesize = filename.size();
            send(client_socket, reinterpret_cast<char*>(&filenamesize), sizeof(filenamesize), 0);

            // �����ļ���
            send(client_socket, filename.c_str(), filenamesize, 0);

            // �����ļ����ݴ�С
            uint64_t filesize;
            ret = recv(client_socket, reinterpret_cast<char*>(&filesize), sizeof(filesize), 0);
            if (ret <= 0) {
                cerr << "Failed to receive file size." << endl;
                return 0;
            }

            // ����Ŀ¼����������ڣ�
            string filepath = "D:/Client/" + filename;
            filesystem::path path(filepath);
            string dir = path.parent_path().string();
            if (!filesystem::exists(dir)) {
                filesystem::create_directories(dir); // ����Ŀ¼
                cout << "Created directory: " << dir << endl;
            }

            // �����ļ�����
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
			cout << "��Ч�����룬�����롰�ϴ��������ء��Լ�����" << endl;
		}
	}
	closesocket(client_socket);
	return 0;
}
