#include "user.h"
#include <iostream>

std::string User::RecvString(SOCKET client_socket)
{
	//��ȡ�ַ�������
	uint64_t stringsize;
	int ret = recv(client_socket, reinterpret_cast<char*>(&stringsize), sizeof(stringsize), 0);
	if (ret <= 0)
		throw(std::runtime_error("Failed to recv stringsize"));
	uint64_t receivedsize = 0;
	char string[1024] = { 0 };
	while (receivedsize < stringsize)
	{
		int ret = recv(client_socket, string, stringsize, 0);
		if (ret <= 0)
			throw(std::runtime_error("Failed to recv string"));
		receivedsize += ret;
	}
	return string;
}
void User::SendString(SOCKET client_socket,std::string string)
{
	uint64_t stringsize=string.size();
	send(client_socket, reinterpret_cast<char*>(&stringsize), sizeof(stringsize), 0);
	send(client_socket, string.c_str(), string.size(), 0);
}
void User::LoginAndRegister(SOCKET client_socket)
{
	if (User::Getchoice(client_socket))
		User::LoginUser(client_socket);
	else
		User::RegisterUser(client_socket);
}
bool User::Getchoice(SOCKET client_socket)
{
	while (1)
	{
		std::cout << "��ѡ���¼����ע��" << std::endl;
		std::string choice;
		std::cin >> choice;
		if (choice == "��¼") {
			SendString(client_socket, choice);
			return true;
		}
		else if (choice == "ע��") {
			SendString(client_socket, choice);
			return false;
		}
		else {
			std::cout << "��Ч�����룬�����롰��¼����ע�ᡱ�Լ�����" << std::endl;
		}		
	}
}
bool User::RegisterUser(SOCKET client_socket)
{
	std::cout << "�������û���" << std::endl;
	std::string username;
	while (1)
	{
		std::cin >> username;
		SendString(client_socket, username);
		std::string ifUserExist = RecvString(client_socket);
		if (ifUserExist == "true") {
			std::cout << "�û����Ѵ���" << std::endl;
		}
		else {
			std::cout << "����������" << std::endl;
			std::string password;
			std::cin >> password;
			SendString(client_socket, password);
			return true;
			std::cout << "ע��ɹ�!" << std::endl;
		}
	}
}
bool User::LoginUser(SOCKET client_socket)
{
	while (1)
	{
		std::string username;
		std::string password;
		std::cout << "�������û���" << std::endl;
		std::cin >> username;
		std::cout << "����������" << std::endl;
		std::cin >> password;
		SendString(client_socket, username);
		SendString(client_socket, password);
		std::string res = RecvString(client_socket);
		if (res == "��½�ɹ�") {
			std::cout << "��½�ɹ�" << std::endl;
			return true;
		}
		else {
			std::cout << "�������" << std::endl;
		}
	}
}
