// WinNetwork.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <vector>
#include "network.h"
using namespace CMCode;

int main()
{
	CMByteArray byeArray("1213131");

	WSADATA wsaData;
	int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret != 0)
	{
		std::cerr << "WSAStartup Error" << "\n";
		return -1;
	}

	SOCKET listenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, NULL, 0);
	if (listenSocket == INVALID_SOCKET)
	{
		std::cerr << "WSASocket:" << WSAGetLastError();
		return -1;
	}

	sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(9999);
	if (inet_pton(AF_INET/*IPv4*/, "127.0.0.1", &addr.sin_addr) != 1)
	{
		std::cerr << "inet_pton:" << WSAGetLastError();
		return -1;
	}

	ret = ::bind(listenSocket, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr));
	if (ret == SOCKET_ERROR)
	{
		std::cerr << "bind:" << WSAGetLastError();
		return -1;
	}
	u_long data = 1;
	if (ioctlsocket(listenSocket, FIONBIO, &data) == SOCKET_ERROR)
	{
		std::cerr << "ioctlsocket:" << WSAGetLastError();
		return -1;
	}

	::listen(listenSocket, SOMAXCONN);

	char buffer[10240]{};

	std::vector<SOCKET> clientVec;
	while (true)
	{
		fd_set readSet{};
		FD_ZERO(&readSet);

		FD_SET(listenSocket, &readSet);

		for (size_t i = 0; i < clientVec.size(); i++)
		{
			FD_SET(clientVec[i], &readSet);
		}

		//timeval timeout{ 0,100 };

		select(1, &readSet, nullptr, nullptr, nullptr);

		//有新连接
		if (FD_ISSET(listenSocket, &readSet))
		{
			SOCKET clientSokcet = accept(listenSocket, nullptr, nullptr);
			if (clientSokcet != INVALID_SOCKET)
			{
				::ioctlsocket(listenSocket, FIONBIO, &data);
				clientVec.push_back(clientSokcet);
				std::cout << "Have a new link" << '\n';
			}
		}

		for (int i = clientVec.size() - 1; i >= 0; i--)
		{
			SOCKET clientSocket = clientVec[i];
			if (FD_ISSET(clientSocket, &readSet))
			{
				int recvSize = recv(clientSocket, buffer, 10240, 0);
				if (recvSize > 0)
				{
					send(clientSocket, buffer, recvSize, 0);
					std::cout << "Reply Client" << '\n';
				}
				else if (recvSize == 0)
				{
					closesocket(clientSocket);
					clientVec.erase(clientVec.begin() + i);
					std::cout << "Client disconnect" << '\n';
				}
				else
				{
					if (WSAGetLastError() == WSAEWOULDBLOCK)
					{
						std::cout << "No message to read" << '\n';
					}
					else
					{
						std::cerr << "recv error";
						closesocket(clientSocket);
						clientVec.erase(clientVec.begin() + i);
					}
				}
			}
		}

	}

	WSACleanup();
}