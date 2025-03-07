#pragma once
#ifndef USERS_H
#define USERS_H

#include <mysql.h>
#include <string>
#include <iostream>
class User {
public:
	static void LoginAndRegister(SOCKET client_socket);
	static bool Getchoice(SOCKET client_socket);
	static bool LoginUser(SOCKET client_socket);
	static bool RegisterUser(SOCKET client_socket);
private:
	static MYSQL* conn;
	static bool ConnectToDatabase();
	static std::string RecvString(SOCKET client_socket);
	static void SendString(SOCKET client_socket, std::string string);
	static bool CheckIfUserExist(const std::string& username);
	static bool CheckPassword(const std::string& username, const std::string& password);
};


#endif