#include "user.h"
#include <iostream>
MYSQL* User::conn = nullptr;
std::string User::RecvString(SOCKET client_socket)
{
	//»ñÈ¡×Ö·û´®³¤¶È
	uint64_t stringsize;
	int ret = recv(client_socket, reinterpret_cast<char*>(&stringsize), sizeof(stringsize), 0);
	if (ret <= 0) {
		std::cout << "ErrorCode:" << GetLastError() << std::endl;
		throw(std::runtime_error("Failed to recv stringsize"));
	}
	uint64_t receivedsize = 0;
	char string[1024] = { 0 };
	while (receivedsize < stringsize)
	{
		int ret = recv(client_socket, string, stringsize, 0);
		if (ret <= 0) {
			std::cout << "ErrorCode:" << GetLastError() << std::endl;
			throw(std::runtime_error("Failed to recv stringsize"));
		}
		receivedsize += ret;
	}
	return string;
}
void User::SendString(SOCKET client_socket, std::string string)
{
	uint64_t stringsize = string.size();
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
	std::string choice = RecvString(client_socket);
	if (choice=="µÇÂ¼")
		return true;
	else
		return false;
}
bool User::RegisterUser(SOCKET client_socket)
{
	if (!ConnectToDatabase()) {
		return false;
	}
	while (1)
	{
		std::string username = RecvString(client_socket);
		std::string ifUserExist;
		if (CheckIfUserExist(username)) {
			ifUserExist = "true";
			SendString(client_socket, ifUserExist);
		}
		else {
			ifUserExist = "false";
			SendString(client_socket, ifUserExist);
			std::string password = RecvString(client_socket);
			std::string query = "INSERT INTO user(username,password) VALUES('" + username + "','" + password + "')";
			if (mysql_query(conn, query.c_str())) {
				std::cout << "INSERT query() failed" << std::endl;
				return false;
			}
			return true;
		}
	}
}
bool User::ConnectToDatabase()
{
	conn = mysql_init(nullptr);
	if (conn == nullptr) {
		std::cerr << "Failed to init MYSQL";
		return false;
	}
	conn = mysql_real_connect(conn, "localhost", "root", "123456", "ConcurrentFileServer", 3306, nullptr, 0);
	if (conn == nullptr) {
		std::cerr << "Failed to realconnect MYSQL";
		return false;
	}
	return true;
}
bool User::CheckIfUserExist(const std::string& username)
{
	std::string query = "SELECT* FROM user WHERE username = '" + username + "'";
	if (mysql_query(conn, query.c_str()))
	{
		std::cerr << "SELECT query() failed\n";
		return FALSE;
	}
	MYSQL_RES* res = mysql_store_result(conn);
	if (res == nullptr)
	{
		std::cerr << "mysql_store_result() failed\n";
		return FALSE;
	}
	return mysql_num_rows(res);
}
bool User::LoginUser(SOCKET client_socket)
{
	if (!ConnectToDatabase()) {
		return false;
	}
	std::string username,password;
	while (1)
	{
		username = RecvString(client_socket);
		password = RecvString(client_socket);
		if (CheckPassword(username, password)) {
			SendString(client_socket, "µÇÂ½³É¹¦");
			return TRUE;
		}
		else {
			SendString(client_socket, "ÃÜÂë´íÎó");
		}
	}
}
bool User::CheckPassword(const std::string& username, const std::string& password)
{
	std::string query = "SELECT password FROM user WHERE username='" + username + "'";
	if (mysql_query(conn, query.c_str())) {
		std::cerr << "SELECT query failed\n";
		return FALSE;
	}
	MYSQL_RES* res = mysql_store_result(conn);
	MYSQL_ROW row = mysql_fetch_row(res);
	std::string storedPassword = row[0];
	return storedPassword == password;
}