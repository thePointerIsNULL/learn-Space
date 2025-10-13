#pragma once
#include "CMByteArray.h"
#include <memory>
#include <sys/types.h>
#include<netinet/in.h>
#include <stack>
#include <vector>
#include <unordered_map>
namespace Reactor
{
	class TcpVirtualSocket
	{
	public:
		struct Error
		{
			int _code = 0;
			CMByteArray _errorStr;
			Error(int code, const CMByteArray& str) :_code(code), _errorStr(str) {};
			Error() = default;
		};
		Error getLastError();
		void close();
	protected:
		TcpVirtualSocket() = default;
		static int createSokcet();
		static int createNoBlockSokcet();
		static void setNoBlock(int fd);
		static void setBlock(int fd);
		int _fd = -1;
		virtual ~TcpVirtualSocket();
		std::stack<Error> _error;

		friend class EchoTcpServer;
		friend class TestTcpClient;
	};
	class TcpSocket : public TcpVirtualSocket
	{
	public:
		TcpSocket(const CMByteArray& ip, u_short port);
		~TcpSocket();
		bool connect();
		bool connect(const CMByteArray& ip, u_short port);
		bool isConnected();
		CMByteArray read(size_t size = -1);
		CMByteArray readAll();
		size_t send(const CMByteArray& data);
	private:
		TcpSocket();
		sockaddr_in _addr;
		bool _isConnect;
		char* _buffer;
		friend class TcpServer;
	};
	using TcpSocketPtr = std::shared_ptr<TcpSocket>;

	class TcpServer : public TcpVirtualSocket
	{
	public:
		TcpServer(const CMByteArray& ip, u_short port);
		~TcpServer();
		bool bind();
		bool listen();
		TcpSocketPtr accept();
	private:
		sockaddr_in _addr;
	};
	using TcpServerPtr = std::shared_ptr<TcpServer>;

	class EchoTcpServer
	{
	public:
		EchoTcpServer(const CMByteArray& ip, const std::vector<u_short>& ports);
		~EchoTcpServer();
		void start();
	private:
		std::unordered_map<int, TcpServerPtr> _tcpServers;
		std::unordered_map<int, TcpSocketPtr> _clSockets;

		int _epollFd;
	};

	class TestTcpClient
	{
	public:
		TestTcpClient(const CMByteArray& ip, const std::vector<u_short>& ports);
		~TestTcpClient();
		void start();
	private:
		std::unordered_map<int, TcpSocketPtr> _clSockets;
		CMByteArray _ip;
		std::vector<u_short> _ports;
		int _epollFd;

	};

}

