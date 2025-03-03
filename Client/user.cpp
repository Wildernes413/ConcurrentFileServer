#include "user.h"
#include <iostream>

std::string User::RecvString(SOCKET client_socket)
{
	//获取字符串长度
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
		std::cout << "请选择登录还是注册" << std::endl;
		std::string choice;
		std::cin >> choice;
		if (choice == "登录") {
			SendString(client_socket, choice);
			return true;
		}
		else if (choice == "注册") {
			SendString(client_socket, choice);
			return false;
		}
		else {
			std::cout << "无效的输入，请输入“登录”或“注册”以继续。" << std::endl;
		}		
	}
}
bool User::RegisterUser(SOCKET client_socket)
{
	std::cout << "请输入用户名" << std::endl;
	std::string username;
	while (1)
	{
		std::cin >> username;
		SendString(client_socket, username);
		std::string ifUserExist = RecvString(client_socket);
		if (ifUserExist == "true") {
			std::cout << "用户名已存在" << std::endl;
		}
		else {
			std::cout << "请输入密码" << std::endl;
			std::string password;
			std::cin >> password;
			SendString(client_socket, password);
			return true;
			std::cout << "注册成功!" << std::endl;
		}
	}
}
bool User::LoginUser(SOCKET client_socket)
{
	while (1)
	{
		std::string username;
		std::string password;
		std::cout << "请输入用户名" << std::endl;
		std::cin >> username;
		std::cout << "请输入密码" << std::endl;
		std::cin >> password;
		SendString(client_socket, username);
		SendString(client_socket, password);
		std::string res = RecvString(client_socket);
		if (res == "登陆成功") {
			std::cout << "登陆成功" << std::endl;
			return true;
		}
		else {
			std::cout << "密码错误" << std::endl;
		}
	}
}
