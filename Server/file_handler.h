#pragma once
#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <WinSock2.h>
#include <string>

void Upload(SOCKET client_socket, const char* filename);
void Download(SOCKET client_socket);

#endif // FILE_HANDLER_H