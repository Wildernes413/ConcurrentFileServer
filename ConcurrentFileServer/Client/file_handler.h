#pragma once
#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <WinSock2.h>
#include <string>
#include <vector>

std::vector<char> ReadFile(const std::string& filename);
void SendFile(SOCKET client_socket, const std::string& filepath);
void ReceiveFile(SOCKET client_socket, const std::string& savepath);

#endif // FILE_HANDLER_H