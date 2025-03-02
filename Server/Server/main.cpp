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
    //��"\"�Ĳ��Ϸ�·���ĳ�"/"�ĺϷ�·������������ļ����������ָ��·��ƴ��
    string serverDir = "D:/Server/";
    string fixdfile = filename;
    replace(fixdfile.begin(), fixdfile.end(), '\\', '/'); // �滻��б��Ϊ��б��
    size_t lastSlash = fixdfile.find_last_of('/'); // �ҵ����һ��б��
    string fileName = (lastSlash == string::npos) ? fixdfile : fixdfile.substr(lastSlash + 1);
    string serverFilePath = serverDir + fileName;

    cout << "Server file path: " << serverFilePath << endl;
    ofstream file(serverFilePath, ios::binary | ios::out);
    if (!file) {
        throw runtime_error("Failed to create file:" + serverFilePath);
    }

    // �����ļ���С
    uint64_t fileSize;
    recv(client_socket, reinterpret_cast<char*>(&fileSize), sizeof(fileSize), 0);

    // �����ļ�����
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
    // չʾ�ļ��ṹ
    string path = "D:/Server";
    string filelist;
    for (const auto& entry : fs::directory_iterator(path))
    {
        filelist += entry.path().filename().string() + "\n";  // ����ļ���
    }

    // �����ļ��б�Ĵ�С
    uint64_t filelistSize = filelist.size();
    send(client_socket, reinterpret_cast<char*>(&filelistSize), sizeof(filelistSize), 0);

    // �����ļ��б�
    send(client_socket, filelist.c_str(), filelistSize, 0);
    cout << "Sent file list: " << filelist << endl;

    // �����ļ�����С
    uint64_t filenamesize;
    int ret = recv(client_socket, reinterpret_cast<char*>(&filenamesize), sizeof(filenamesize), 0);
    if (ret <= 0) {
        cerr << "Failed to receive filename size." << endl;
        return;
    }

    // �����ļ���
    vector<char> filenameBuffer(filenamesize + 1); // ��̬���仺��������һ���ֽ�������ֹ��
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
    filenameBuffer[filenamesize] = '\0'; // �ֶ������ֹ��
    string filename(filenameBuffer.data()); // ������������ת��Ϊ std::string

    // �����ļ����ݴ�С
    string filepath = "D:/Server/" + filename;
    ifstream file(filepath, ios::binary | ios::ate);
    if (!file) {
        throw runtime_error("Failed to open the file: " + filepath);
    }

    // ��ȡ�ļ���С
    streamsize filesize = file.tellg();

    // �ƶ����ļ���ͷ
    file.seekg(0, ios::beg);
    vector<char> filecontent(filesize);
    if (!file.read(filecontent.data(), filesize)) {
        throw runtime_error("Failed to read the file: " + filepath);
    }

    // �����ļ���С
    send(client_socket, reinterpret_cast<char*>(&filesize), sizeof(filesize), 0);

    // �����ļ�����
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

        if (strcmp(rbuffer, "�ϴ��ļ�") == 0) {
            ret = recv(client_socket, rbuffer, sizeof(rbuffer), 0);
            if (ret <= 0) break;

            Upload(client_socket, rbuffer);
        }
        if (strcmp(rbuffer, "�����ļ�") == 0) {
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
	//��������Ȩ��
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	//����socket�׽���
	SOCKET listen_scoket = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_scoket == INVALID_SOCKET) {
		cout << "Create Socket Failed! Errcode:" << GetLastError() << endl;
		return -1;
	}
	//����˿ں�
	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons(8080);
	local.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(listen_scoket, (const sockaddr*)&local, sizeof(local))==-1) {
		cout << "Bind Sokcet Failed! Errcode:" << GetLastError() << endl;
		return -1;
	}
	//��ʼ����
	if (listen(listen_scoket, 10) == -1) {
		cout << "Listen Scoket Failed! Errcode:" << GetLastError() << endl;
		return -1;
	}
	//�ȴ��ͻ�������
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
