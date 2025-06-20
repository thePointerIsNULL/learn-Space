#include "iocp.h"
#include<thread>

namespace CMCode
{
	CMIOCP::CMIOCP()
	{

	}

	CMIOCP::~CMIOCP()
	{
		close();
	}

	void CMIOCP::startIocp(CMHostAddress address, ushort port, IOResultFun registerFun, int threadCount /*= 0*/)
	{
		close();
		m_acceptEx = nullptr;
		m_getAcceptExAddrs = nullptr;

		m_iocpManger.iocp = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);

		SOCKET listenSocket = ::WSASocket(AF_INET, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);
		if (listenSocket == INVALID_SOCKET)
		{
			return;
		}
		sockaddr_in sockeAddr{ 0 };
		sockeAddr.sin_family = AF_INET;
		sockeAddr.sin_port = htons(9999);
		sockeAddr.sin_addr.s_addr = address.ip();

		::bind(listenSocket, reinterpret_cast<sockaddr*>(&sockeAddr), sizeof(sockaddr));
		::listen(listenSocket, SOMAXCONN);

		m_iocpManger.listenSocket = listenSocket;
		m_iocpManger.callbackFun = registerFun;
		m_iocpManger.thisPtr = this;

		if (threadCount == 0)
		{
			threadCount = std::thread::hardware_concurrency() * 0.8;
		}
		m_iocpManger.threadFlage.store(true);

		for (size_t i = 0; i < threadCount; i++)
		{
			m_iocpManger.threadPool.emplace_back(std::thread(&workFun, &m_iocpManger));
		}

		::CreateIoCompletionPort(reinterpret_cast<HANDLE>(listenSocket), m_iocpManger.iocp, static_cast<ULONG_PTR>(listenSocket), 0);

		GUID acceptExGuid = WSAID_ACCEPTEX;
		DWORD byte = 0;
		::WSAIoctl(listenSocket
			, SIO_GET_EXTENSION_FUNCTION_POINTER
			, &acceptExGuid
			, sizeof(acceptExGuid),
			&m_acceptEx
			, sizeof(m_acceptEx), &byte, 0, 0);

		acceptExGuid = WSAID_GETACCEPTEXSOCKADDRS;
		::WSAIoctl(listenSocket
			, SIO_GET_EXTENSION_FUNCTION_POINTER
			, &acceptExGuid
			, sizeof(acceptExGuid),
			&m_getAcceptExAddrs
			, sizeof(m_getAcceptExAddrs), &byte, 0, 0);


		for (size_t i = 0; i < m_iocpManger.threadPool.size() / 2; i++)
		{
			SOCKET clntSokcet = ::WSASocket(AF_INET, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);

			IOContext* acceptContext = newIOContext();
			acceptContext->ioMode = IOMode::Accept;
			acceptContext->clntSocket = clntSokcet;

			DWORD trByte = 0;
			m_acceptEx(listenSocket
				, clntSokcet
				, acceptContext->buffer.buf
				, 0
				, sizeof(sockaddr_in) + 16
				, sizeof(sockaddr_in) + 16
				, &trByte
				, &acceptContext->overlappen);
		}
	}

	void CMIOCP::send(CMTcpSocketPtr socket, const CMByteArray& msg)
	{
		IOContext* ioContext = newIOContext();
		ioContext->ioMode = IOMode::Write;
		const char* data = nullptr;
		uint dataSize = 0;
		msg.data(data, dataSize);
		std::memcpy(ioContext->buffer.buf, data, dataSize);
		ioContext->buffer.len = dataSize;
		DWORD trSize = 0;
		DWORD flage = 0;
		::WSASend(socket->m_socket, &ioContext->buffer, 1, &trSize, flage, &ioContext->overlappen, 0);
	}

	void CMIOCP::send(CMTcpSocketPtr socket, char* str)
	{
		IOContext* ioContext = newIOContext();
		ioContext->ioMode = IOMode::Write;
		ioContext->buffer.len = strlen(str);
		std::memcpy(ioContext->buffer.buf, str, ioContext->buffer.len);
		
		DWORD trSize = 0;
		DWORD flage = 0;
		::WSASend(socket->m_socket, &ioContext->buffer, 1, &trSize, flage, &ioContext->overlappen, 0);
	}

	void CMIOCP::close()
	{
		clearConnected();
		::Sleep(10);
		clearThread();
	}

	CMCode::CMIOCP::IOContext* CMIOCP::newIOContext()
	{
		IOContext* context = new IOContext;
		context->realityBuffer = new char[10240] {0};
		context->buffer.buf = context->realityBuffer;
		context->buffer.len = 10240;
		return context;
	}


	void CMIOCP::deleteIOContext(IOContext* context)
	{
		delete[]context->realityBuffer;
		delete context;
	}

	void CMIOCP::clearConnected()
	{
		for (size_t i = 0; i < m_iocpManger.clntSocketVec.size(); i++)
		{
			::closesocket(m_iocpManger.clntSocketVec[i]);
		}
		m_iocpManger.clntSocketVec.clear();
	}

	void CMIOCP::clearThread()
	{
		m_iocpManger.threadFlage.store(false);
		for (size_t i = 0; i < m_iocpManger.threadPool.size(); i++)
		{
			if (m_iocpManger.threadPool[i].joinable())
			{
				m_iocpManger.threadPool[i].join();
			}
		}
		m_iocpManger.threadPool.clear();
	}

	void CMIOCP::workFun(IOCPManger* iocpManger)
	{
		DWORD trByteSize = 0;
		ULONG_PTR key = 0;
		LPOVERLAPPED overlapped = nullptr;
		CMIOCP* iocpThis = iocpManger->thisPtr;

		while (iocpManger->threadFlage)
		{
			BOOL ret = ::GetQueuedCompletionStatus(iocpManger->iocp
				, &trByteSize
				, &key
				, &overlapped
				, INFINITE);

			if (key == 0
				&& overlapped == nullptr)
			{
				int error = ::WSAGetLastError();
				continue;
			}

			SocketContext* socketContext = reinterpret_cast<SocketContext*>(key);
			IOContext* ioContext = reinterpret_cast<IOContext*>(overlapped);

			if (!ret)
			{
				IOResult result;
				result.socket = socketContext->clntSocket;
				iocpManger->callbackFun(&result);
				::closesocket(result.socket->getSocket());
				iocpThis->deleteIOContext(ioContext);
				delete socketContext;
				break;
			}

			switch (ioContext->ioMode)
			{
			case IOMode::Accept:
			{
				SOCKET clntSocket = ioContext->clntSocket;

				int ret = setsockopt(clntSocket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, reinterpret_cast<char*>(&iocpManger->listenSocket), sizeof(SOCKET));

				iocpManger->clntSocketVec.push_back(clntSocket);

				{
					SocketContext* context = new SocketContext;

					sockaddr* addr = nullptr;
					int addrSize = 0;
					sockaddr* remtoAddr = nullptr;
					int reAddrSize = 0;

					iocpThis->m_getAcceptExAddrs(ioContext->buffer.buf
						, 0
						, sizeof(sockaddr_in) + 16
						, sizeof(sockaddr_in) + 16
						, &addr
						, &addrSize
						, &remtoAddr
						, &reAddrSize);
					context->clntSocket = std::make_shared<CMTcpSocket>();
					context->clntSocket->m_socket = clntSocket;
					context->addr = *reinterpret_cast<sockaddr_in*>(addr);

					::CreateIoCompletionPort(reinterpret_cast<HANDLE>(clntSocket), iocpManger->iocp, reinterpret_cast<ULONG_PTR>(context), 0);

					IOResult result;
					result.ioMode = IOMode::Accept;
					result.socket = context->clntSocket;
					iocpManger->callbackFun(&result);
				}


				{
					IOContext* readContext = iocpThis->newIOContext();
					readContext->ioMode = IOMode::Read;
					DWORD trSize = 0;
					DWORD flage = 0;
					readContext->clntSocket = clntSocket;
					::WSARecv(clntSocket, &readContext->buffer, 1, &trSize, &flage, &readContext->overlappen, 0);
				}

				{
					SOCKET newClntSocket = ::WSASocket(AF_INET, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);
					ioContext->clntSocket = newClntSocket;

					DWORD size = 0;
					iocpThis->m_acceptEx(iocpManger->listenSocket
						, newClntSocket
						, ioContext->realityBuffer
						, 0
						, sizeof(sockaddr_in) + 16
						, sizeof(sockaddr_in) + 16
						, &size
						, &ioContext->overlappen);
				}
			}
			break;
			case IOMode::Read:
			{
				if (trByteSize == 0)
				{
					::closesocket(ioContext->clntSocket);

					auto itor = std::find(iocpManger->clntSocketVec.begin(), iocpManger->clntSocketVec.end(), ioContext->clntSocket);
					iocpManger->clntSocketVec.erase(itor);

					iocpThis->deleteIOContext(ioContext);
					delete socketContext;
					continue;
				}

				IOResult result;
				result.ioMode = IOMode::Read;
				//result.msg = CMByteArray(ioContext->realityBuffer, trByteSize);

				if (socketContext->clntSocket != nullptr)
				{
					result.socket = socketContext->clntSocket;
				}

				iocpManger->callbackFun(&result);

				std::memset(ioContext->realityBuffer, 0, ioContext->buffer.len);
				DWORD trSize = 0;
				DWORD flage = 0;
				ioContext->overlappen = {};
				ioContext->ioMode = IOMode::Read;

				::WSARecv(ioContext->clntSocket, &ioContext->buffer, 1, &trSize, &flage, &ioContext->overlappen, 0);
			}
			break;
			case IOMode::Write:
			{
				if (trByteSize == 0)
				{
					::closesocket(ioContext->clntSocket);

					auto itor = std::find(iocpManger->clntSocketVec.begin(), iocpManger->clntSocketVec.end(), ioContext->clntSocket);
					iocpManger->clntSocketVec.erase(itor);

					delete socketContext;
				}
				iocpThis->deleteIOContext(ioContext);
			}
			break;
			default:
				break;
			}

		}
	}

};