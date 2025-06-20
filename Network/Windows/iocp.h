#pragma once
#include "network.h"
#include <vector>
#include <functional>
#include <atomic>
#include <thread>
#include <mswsock.h>

namespace CMCode
{
	class CMIOCP
	{
	public:
		enum class IOMode
		{
			None,
			Read,
			Write,
			Accept
		};
		struct IOResult
		{
			IOMode ioMode = IOMode::None;
			CMTcpSocketPtr socket;
			CMByteArray msg;
		};

		struct IOContext
		{
			OVERLAPPED overlappen{};//一定要初始化，不初始化害人
			IOMode ioMode = IOMode::None;
			char* realityBuffer;
			WSABUF buffer;
			SOCKET clntSocket;
		};
		struct SocketContext
		{
			CMTcpSocketPtr clntSocket;
			sockaddr_in addr;
		};

		using IOResultFun = std::function<void(IOResult*)>;

		CMIOCP();
		~CMIOCP();
		void startIocp(CMHostAddress address, ushort port, IOResultFun registerFun, int threadCount = 0);
		void send(CMTcpSocketPtr socket, const CMByteArray& msg);
		void send(CMTcpSocketPtr socket, char* str);
		void close();
	private:
		IOContext* newIOContext();
		void deleteIOContext(IOContext* context);
		void clearConnected();
		void clearThread();
	private:
		struct IOCPManger
		{
			HANDLE iocp;
			SOCKET listenSocket = INVALID_SOCKET;
			std::vector<std::thread> threadPool;
			std::vector<SOCKET> clntSocketVec;
			std::atomic_bool threadFlage{ false };
			IOResultFun callbackFun;
			CMIOCP* thisPtr;

		}m_iocpManger{};

		LPFN_ACCEPTEX m_acceptEx = nullptr;
		LPFN_GETACCEPTEXSOCKADDRS m_getAcceptExAddrs = nullptr;
		static void workFun(IOCPManger* icopManger);

	};

}


