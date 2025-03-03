#pragma once
#ifndef USERS_H
#define USERS_H
#include <string>
#include <iostream>
#include <WinSock2.h>
class User {
public:
	static void LoginAndRegister(SOCKET client_socket);
	static bool Getchoice(SOCKET client_socket);
	static bool LoginUser(SOCKET client_socket);
	static bool RegisterUser(SOCKET client_socket);
private:
	static std::string RecvString(SOCKET client_socket);
	static void SendString(SOCKET client_socket, std::string string);
};

#endif 
