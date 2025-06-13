// WinNetwork.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <vector>
#include <thread>
#include <mswsock.h>
#include "network.h"
using namespace CMCode;

LPFN_ACCEPTEX acceptExFun = nullptr;
LPFN_GETACCEPTEXSOCKADDRS getClntAddrFun = nullptr;

enum class IOMode
{
	None,
	Read,
	Write,
	Accept
};

struct IOContext
{
	OVERLAPPED overlapped{};
	WSABUF buffer;
	ulong bufferDefSize = 10240;
	ulong vaildSize = 0;//表示buffer中实际的数据大小，accept事件时没有意义
	IOMode mode = IOMode::None;
	SOCKET socket;
};

void initIOContext(IOContext* iOContext)
{
	iOContext->buffer.buf = new CHAR[iOContext->bufferDefSize];
	iOContext->buffer.len = iOContext->bufferDefSize;
}

void freeIOContext(IOContext* iOContext)
{
	delete[]iOContext->buffer.buf;
	iOContext->buffer.buf = nullptr;
}

struct SocketContext
{
	SOCKET socket = INVALID_SOCKET;
	sockaddr_in socketAddr{};
};


void workFun(void* arg)
{
	HANDLE ioObj = reinterpret_cast<HANDLE>(arg);
	SocketContext* socketContext = nullptr;
	IOContext* ioContext = nullptr;
	DWORD trByteSize = 0;
	char revcHeader[] = "recv:";
	while (true)
	{
		bool ret = ::GetQueuedCompletionStatus(ioObj
			, &trByteSize
			, reinterpret_cast<PULONG_PTR>(&socketContext)
			, reinterpret_cast<LPOVERLAPPED*>(&ioContext)
			, INFINITE);

		if (!ret)
		{
			std::cout << "Socket error;" << std::endl;
			closesocket(socketContext->socket);
			freeIOContext(ioContext);
			delete ioContext;
			delete socketContext;
			continue;
		}

		if (reinterpret_cast<ULONG_PTR>(socketContext) == 0xffffffff)
		{
			/*closesocket(socketContext->socket);
			freeIOContext(ioContext);
			delete ioContext;
			delete socketContext;*/
			break;
		}

		switch (ioContext->mode)
		{
		case IOMode::Accept:
		{
			//更新新连接的socket信息，与listenSocket关联
			SOCKET clntSocket = ioContext->socket;
			if (clntSocket == INVALID_SOCKET)
			{
				std::cerr << "error Socket;" << std::endl;
			}
			SOCKET listenSocket = reinterpret_cast<SOCKET>(socketContext);

			int ret = setsockopt(clntSocket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, reinterpret_cast<char*>(&listenSocket), sizeof(SOCKET));
			int error = WSAGetLastError();

			sockaddr* addr = nullptr;
			int addrSize = 0;
			sockaddr* remtoAddr = nullptr;
			int reAddrSize = 0;
			getClntAddrFun(ioContext->buffer.buf
				, 0
				, sizeof(sockaddr) + 16
				, sizeof(sockaddr) + 16
				, &addr, &addrSize
				, &remtoAddr, &reAddrSize);

			SocketContext* newClntSocketContext = new SocketContext();
			newClntSocketContext->socket = clntSocket;
			newClntSocketContext->socketAddr = *reinterpret_cast<sockaddr_in*>(addr);

			//关联完成端口
			::CreateIoCompletionPort(reinterpret_cast<HANDLE>(clntSocket), ioObj, reinterpret_cast<ULONG_PTR>(newClntSocketContext), 0);

			/*	freeIOContext(ioContext);
				delete ioContext;*/

				//请求读数据
			IOContext* readIoContext = new IOContext();
			initIOContext(readIoContext);
			readIoContext->mode = IOMode::Read;
			DWORD trSize = 0;
			DWORD flage = 0;

			::WSARecv(clntSocket, &readIoContext->buffer, 1, &trSize, &flage, &readIoContext->overlapped, nullptr);

			//请求新的连接
			IOContext* acceptIoContext = new IOContext();
			initIOContext(acceptIoContext);
			acceptIoContext->mode = IOMode::Accept;


			SOCKET newClnt = WSASocket(AF_INET, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);
			DWORD trByte = 0;
			/*bool ret =*/ acceptExFun(listenSocket
				, newClnt
				, acceptIoContext->buffer.buf
				, 0/*0表示不接受数据，非0时 有链接后会等待到有数据接收时才唤醒，此函数可以控制过滤一些只连接但不通信的客户端*/
				, sizeof(sockaddr) + 16
				, sizeof(sockaddr) + 16
				, &trByte
				, &acceptIoContext->overlapped);
		}
		break;
		case IOMode::Read:
		{
			if (trByteSize == 0)
			{
				::closesocket(socketContext->socket);
				freeIOContext(ioContext);
				delete ioContext;
				continue;
			}
			CMByteArray recvByte(ioContext->buffer.buf, trByteSize);
			recvByte.insert(0, revcHeader);

			std::memset(ioContext->buffer.buf, 0, ioContext->bufferDefSize);
			DWORD flage = 0;
			DWORD trSize = 0;
			::WSARecv(socketContext->socket, &ioContext->buffer, 1, &trSize, &flage, reinterpret_cast<OVERLAPPED*>(&ioContext->overlapped), 0);


			IOContext* sendIo = new IOContext;
			initIOContext(sendIo);
			sendIo->mode = IOMode::Write;
			const char* data = nullptr;
			uint dataSize = 0;
			recvByte.data(data, dataSize);
			std::memcpy(sendIo->buffer.buf, data, dataSize);
			sendIo->buffer.len = dataSize;
			::WSASend(socketContext->socket,&sendIo->buffer,1,&trSize,flage, reinterpret_cast<OVERLAPPED*>(&sendIo->overlapped), 0);
		}
		break;
		case IOMode::Write:
		{
			if (trByteSize == 0)
			{
				::closesocket(socketContext->socket);
			}
			freeIOContext(ioContext);
			delete ioContext;
		}
			break;
		default:
			break;
		}
	}
}

int main()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);


	//IOCP
	/*SYSTEM_INFO systemInfor{};
	::GetSystemInfo(&systemInfor);
	uint threadCount = systemInfor.dwNumberOfProcessors;*/
	uint threadCount = std::thread::hardware_concurrency();



	HANDLE ioObj = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);


	SOCKET listenSocket = ::WSASocket(AF_INET, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);
	if (listenSocket == INVALID_SOCKET)
	{
		return -1;
	}
	sockaddr_in sockeAddr{ 0 };
	sockeAddr.sin_family = AF_INET;
	sockeAddr.sin_port = htons(9999);
	sockeAddr.sin_addr.s_addr = 0;

	int ret = ::bind(listenSocket, reinterpret_cast<sockaddr*>(&sockeAddr), sizeof(sockaddr));
	ret = ::listen(listenSocket, SOMAXCONN);


	std::vector<std::thread> threadPool;

	for (size_t i = 0; i < threadCount; i++)
	{
		threadPool.emplace_back(std::thread(&workFun, ioObj));
	}

	/*SocketContext* listenSocketContext = new SocketContext;
	listenSocketContext->socket = listenSocket;*/

	::CreateIoCompletionPort(reinterpret_cast<HANDLE>(listenSocket), ioObj, static_cast<ULONG_PTR>(listenSocket), 0);


	//查询acceptEx
	GUID acceptExGuid = WSAID_ACCEPTEX;
	DWORD byte = 0;
	::WSAIoctl(listenSocket
		, SIO_GET_EXTENSION_FUNCTION_POINTER
		, &acceptExGuid
		, sizeof(acceptExGuid),
		&acceptExFun
		, sizeof(acceptExFun), &byte, 0, 0);

	acceptExGuid = WSAID_GETACCEPTEXSOCKADDRS;
	::WSAIoctl(listenSocket
		, SIO_GET_EXTENSION_FUNCTION_POINTER
		, &acceptExGuid
		, sizeof(acceptExGuid),
		&getClntAddrFun
		, sizeof(getClntAddrFun), &byte, 0, 0);

	//投递异步的accept
	for (size_t i = 0; i < threadCount / 2; i++)
	{
		SOCKET clntSocket = ::WSASocket(AF_INET, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);

		IOContext* acceptIoContext = new IOContext();
		initIOContext(acceptIoContext);
		acceptIoContext->mode = IOMode::Accept;
		acceptIoContext->socket = clntSocket;

		DWORD trByte = 0;
		bool ret = acceptExFun(listenSocket
			, clntSocket
			, acceptIoContext->buffer.buf
			, 0/*0表示不接受数据，非0时 有链接后会等待到有数据接收时才唤醒，此函数可以控制过滤一些只连接但不通信的客户端*/
			, sizeof(sockaddr) + 16
			, sizeof(sockaddr) + 16
			, &trByte
			, &acceptIoContext->overlapped);
	}


	std::cin.get();


	for (size_t i = 0; i < threadCount; i++)
	{
		::PostQueuedCompletionStatus(ioObj, 0, 0xffffffff, nullptr);
	}

	for (size_t i = 0; i < threadPool.size(); i++)
	{
		if (threadPool[i].joinable())
		{
			threadPool[i].join();
		}
	}

	::closesocket(listenSocket);


	return 0;

	while (true)
	{
		sockaddr_in addr{ 0 };
		int addrLen = sizeof(sockaddr_in);
		SOCKET clntSocket = ::accept(listenSocket, reinterpret_cast<sockaddr*>(&addr), &addrLen);
		if (clntSocket == INVALID_SOCKET)
		{
			continue;
		}

		SocketContext* socketContext = new SocketContext;
		socketContext->socket = clntSocket;
		socketContext->socketAddr = addr;

		::CreateIoCompletionPort(reinterpret_cast<HANDLE>(clntSocket), ioObj, reinterpret_cast<ULONG_PTR>(socketContext), 0);

		IOContext* ioContext = new IOContext();
		initIOContext(ioContext);
		ioContext->mode = IOMode::Read;


		DWORD flags = 0;
		DWORD bytesRecvd = 0;

		int ret = ::WSARecv(clntSocket
			, &ioContext->buffer
			, 1
			, &bytesRecvd
			, &flags
			, &ioContext->overlapped
			, nullptr);

		if (ret == SOCKET_ERROR)
		{
			int error = ::WSAGetLastError();
			if (error != WSA_IO_PENDING)
			{
				::closesocket(clntSocket);
				freeIOContext(ioContext);
				delete socketContext;
				delete ioContext;
			}
		}
	}
















	WSACleanup();

	return 0;
	//{
	//	CMTcpServer tcpServer;
	//	tcpServer.bind(CMHostAddress::LocalHost, 9999);
	//	if (!tcpServer.listen())
	//	{
	//		std::cerr << tcpServer.getLastError() << '\n';
	//		return -1;
	//	}

	//	std::vector<CMTcpSocketPtr> clientSockets;

	//	CMSelect::MonitorMap monitorSocketMap;
	//	monitorSocketMap[&tcpServer] = CMSelect::EventType::ReadEvent;

	//	CMSelect selectMgr;
	//	selectMgr.setMonitorMap(monitorSocketMap);

	//	while (true)
	//	{
	//		uint timeout = 100;
	//		if (!selectMgr.doRun(&timeout))
	//		{
	//			continue;
	//		}

	//		CMSelect::MonitorMap monitorResultMap;
	//		selectMgr.getResult(monitorResultMap);

	//		for (const auto& var : monitorResultMap)
	//		{
	//			CMSocket* socket = var.first;
	//			if (socket == &tcpServer)
	//			{
	//				CMTcpSocketPtr scoket = tcpServer.accept();
	//				if (scoket != nullptr)
	//				{
	//					clientSockets.push_back(scoket);
	//					selectMgr.updateSocketEvent(scoket.get(), CMSelect::EventType::ReadEvent);
	//				}
	//				continue;
	//			}
	//			else
	//			{
	//				CMByteArray msgByte = socket->readAll();
	//				if (msgByte.isEmpty()
	//					|| !socket->isValidSocket())
	//				{
	//					selectMgr.removeSocket(socket);
	//					std::cerr << "dis" << '\n';
	//					continue;
	//				}
	//				msgByte.insert(0, "revc:");
	//				socket->sendAll(msgByte);
	//			}

	//		}

	//		/*CMTcpSocketPtr clientSocket = tcpServer.accept();
	//		if (clientSocket != nullptr)
	//		{
	//			clientSockets.push_back(clientSocket);
	//		}


	//		for (int i = clientSockets.size() - 1; i >= 0; i--)
	//		{
	//			CMTcpSocketPtr& socket = clientSockets[i];
	//			if (!socket->isValidSocket())
	//			{
	//				clientSockets.erase(clientSockets.begin() + i);
	//				continue;
	//			}
	//			CMByteArray msgByte = socket->readAll();
	//			if (msgByte.isEmpty())
	//			{
	//				continue;
	//			}
	//			msgByte.insert(0, "revc:");
	//			socket->sendAll(msgByte);
	//		}*/

	//		Sleep(1);
	//	}
	//}


	return 0;


	/*WSADATA wsaData;
	ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret != 0)
	{
		std::cerr << "WSAStartup Error" << "\n";
		return -1;
	}*/

	//CMHostAddress hostAddr = CMHostAddress::fromHostName("www.14997.com");
	//if (!hostAddr.isNull())
	//{
	//	std::cout << hostAddr.toString() << '\n';
	//}

	//CMHostAddress  aaaa("127.0.0.1");
	//std::cout << aaaa.toString() << '\n';


	//CMByteArray a;
	//{
	//	CMByteArray byeArray("1213131");

	//	a = byeArray;

	//	a.append("oooo");

	//	const char* str = nullptr;
	//	uint len = 0;
	//	a.data(str, len);
	//	std::cout << str;

	//	CMByteArray b(byeArray);
	//}

	///*const char* str = nullptr;
	//int len = 0;
	//a.data(str, len);
	//std::cout << str;*/

	//CMSelect::EventType type = CMSelect::EventType::ReadEvent | CMSelect::EventType::WriteEvent;


	//SOCKET listenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, NULL, 0);
	//if (listenSocket == INVALID_SOCKET)
	//{
	//	std::cerr << "WSASocket:" << WSAGetLastError();
	//	return -1;
	//}
	//u_long data = 1;
	//if (ioctlsocket(listenSocket, FIONBIO, &data) == SOCKET_ERROR)
	//{
	//	std::cerr << "ioctlsocket:" << WSAGetLastError();
	//	return -1;
	//}

	//sockaddr_in addr{};
	//addr.sin_family = AF_INET;
	//addr.sin_port = htons(9999);
	//if (inet_pton(AF_INET/*IPv4*/, "127.0.0.1", &addr.sin_addr) != 1)
	//{
	//	std::cerr << "inet_pton:" << WSAGetLastError();
	//	return -1;
	//}

	//int ret = ::bind(listenSocket, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr));
	//if (ret == SOCKET_ERROR)
	//{
	//	std::cerr << "bind:" << WSAGetLastError();
	//	return -1;
	//}


	//::listen(listenSocket, SOMAXCONN);

	//char buffer[10240]{};

	//std::vector<SOCKET> clientVec;
	//while (true)
	//{
	//	fd_set readSet{};
	//	FD_ZERO(&readSet);

	//	FD_SET(listenSocket, &readSet);

	//	for (size_t i = 0; i < clientVec.size(); i++)
	//	{
	//		FD_SET(clientVec[i], &readSet);
	//	}

	//	//timeval timeout{ 0,100 };

	//	select(1, &readSet, nullptr, nullptr, nullptr);

	//	//有新连接
	//	if (FD_ISSET(listenSocket, &readSet))
	//	{
	//		SOCKET clientSokcet = accept(listenSocket, nullptr, nullptr);
	//		if (clientSokcet != INVALID_SOCKET)
	//		{
	//			::ioctlsocket(listenSocket, FIONBIO, &data);
	//			clientVec.push_back(clientSokcet);
	//			std::cout << "Have a new link" << '\n';
	//		}
	//	}

	//	/*for (int i = clientVec.size() - 1; i >= 0; i--)
	//	{
	//		SOCKET clientSocket = clientVec[i];
	//		if (FD_ISSET(clientSocket, &readSet))
	//		{
	//			int recvSize = recv(clientSocket, buffer, 10240, 0);
	//			if (recvSize > 0)
	//			{
	//				send(clientSocket, buffer, recvSize, 0);
	//				std::cout << "Reply Client" << '\n';
	//			}
	//			else if (recvSize == 0)
	//			{
	//				closesocket(clientSocket);
	//				clientVec.erase(clientVec.begin() + i);
	//				std::cout << "Client disconnect" << '\n';
	//			}
	//			else
	//			{
	//				if (WSAGetLastError() == WSAEWOULDBLOCK)
	//				{
	//					std::cout << "No message to read" << '\n';
	//				}
	//				else
	//				{
	//					std::cerr << "recv error";
	//					closesocket(clientSocket);
	//					clientVec.erase(clientVec.begin() + i);
	//				}
	//			}
	//		}
	//	}*/

	//}

	//WSACleanup();
}