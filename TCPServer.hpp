#pragma once

#include <iostream>
#include <thread>
#include <atomic> 

#include <ws2tcpip.h>
#pragma comment (lib, "ws2_32.lib")

#ifdef UNIOCODE
#define outErr std::wcerr
#else
#define outErr std::cerr
#endif


class TCPServer
{
public:
	TCPServer(const int port = 12345);
	~TCPServer();

	void Run();
	void Stop();

private:
	void PrintWSAErrorAsString(DWORD errorCode);
	void HandleClient(SOCKET clientSocket);
	void StartTCPServer();

	WSADATA wsData;
	WORD version;
	const int port;
	SOCKET listening;

	std::atomic<bool> stopServer;
};