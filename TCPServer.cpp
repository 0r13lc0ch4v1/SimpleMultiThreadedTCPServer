#include "TCPServer.hpp"


TCPServer::TCPServer(const int port) :
	port(port)
{
	this->stopServer = false;
	this->version = MAKEWORD(2, 2);
	this->wsData = { 0 };
	this->listening = { 0 };
}

TCPServer::~TCPServer()
{
	closesocket(this->listening);

	WSACleanup();
}

void TCPServer::StartTCPServer()
{
	int wsOK = WSAStartup(this->version, &this->wsData);
	if (0 != wsOK)
	{
		outErr << "Can't initialize winsock!" << std::endl;
		this->PrintWSAErrorAsString(wsOK);
		return;
	}

	this->listening = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == this->listening)
	{
		outErr << "Can't create a socket! Quitting" << std::endl;
		return;
	}

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(this->listening, (sockaddr*)&hint, sizeof(hint));

	listen(this->listening, SOMAXCONN);

	while (!this->stopServer)
	{
		sockaddr_in client;
		int clientSize = sizeof(client);

		SOCKET clientSocket = accept(this->listening, (sockaddr*)&client, &clientSize);
		if (INVALID_SOCKET == clientSocket) continue;

		char host[NI_MAXHOST];
		char service[NI_MAXSERV];

		ZeroMemory(host, NI_MAXHOST);
		ZeroMemory(service, NI_MAXSERV);

		if (0 == getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0))
		{
			std::cout << host << " connected on port " << service << std::endl;
		}
		else
		{
			inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
			std::cout << host << " connected on port " << ntohs(client.sin_port) << std::endl;
		}

		std::thread runClient(&TCPServer::HandleClient, this, clientSocket);
		runClient.detach();
	}
}

void TCPServer::PrintWSAErrorAsString(DWORD errorCode)
{
	PTCHAR errMsg = NULL;
	DWORD err = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&errMsg, 0, NULL);
	if (0 == err)
		outErr << TEXT("FormatMessage Failed! GLE=0x") << std::hex << GetLastError() << std::endl;
	else
		outErr << errMsg << std::endl;

	LocalFree(errMsg);
}

void TCPServer::HandleClient(const SOCKET clientSocket)
{
	char buf[4096];
	while (!this->stopServer)
	{
		ZeroMemory(buf, 4096);

		int bytesReceived = recv(clientSocket, buf, 4096, 0);
		if (SOCKET_ERROR == bytesReceived)
		{
			std::cerr << "Error in revc()! Quitting" << std::endl;
			break;
		}
		if (0 == bytesReceived)
		{
			std::cout << "Client disconnected " << std::endl;
			break;
		}

		send(clientSocket, buf, bytesReceived + 1, 0);
	}

	closesocket(clientSocket);
}

void TCPServer::Stop()
{
	this->stopServer = true;
}

void TCPServer::Run()
{
	std::thread runServer(&TCPServer::StartTCPServer, this);
	runServer.detach();
}