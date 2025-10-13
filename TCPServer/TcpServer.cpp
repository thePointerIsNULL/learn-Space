#include "TcpServer.h"
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include<arpa/inet.h>
#include <errno.h>
#include <sys/epoll.h>
#include <iostream>
#include <string.h>
namespace Reactor
{
	TcpVirtualSocket::~TcpVirtualSocket()
	{
		close();
	}

	void TcpVirtualSocket::close()
	{
		std::stack<Error> emptyS;
		_error.swap(emptyS);
		if (_fd != -1)
		{
			::close(_fd);
			_fd = -1;
		}
	}

	TcpVirtualSocket::Error TcpVirtualSocket::getLastError()
	{
		if (!_error.empty())
		{
			Error error = _error.top();
			_error.pop();
			return error;
		}

		return {};
	}

	int TcpVirtualSocket::createSokcet()
	{
		int fd = ::socket(AF_INET, SOCK_STREAM, 0);

		timeval timeout{};
		timeout.tv_sec = 0;
		timeout.tv_usec = 200;
		int ret = setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

		return fd;
	}
	int TcpVirtualSocket::createNoBlockSokcet()
	{
		int fd = ::socket(AF_INET, SOCK_STREAM, 0);
		int optVal = ::fcntl(fd, F_GETFL, 0);
		optVal |= O_NONBLOCK;
		::fcntl(fd, F_SETFL, optVal);
		return fd;
	}

	void TcpVirtualSocket::setNoBlock(int fd)
	{
		int optVal = ::fcntl(fd, F_GETFL, 0);
		optVal |= O_NONBLOCK;
		::fcntl(fd, F_SETFL, optVal);
	}

	void TcpVirtualSocket::setBlock(int fd)
	{
		int optVal = ::fcntl(fd, F_GETFL, 0);
		optVal &= ~O_NONBLOCK;
		::fcntl(fd, F_SETFL, optVal);
	}

	TcpSocket::TcpSocket(const CMByteArray& ip, u_short port)
		: _isConnect(false)
	{
		_addr = { 0 };
		_addr.sin_family = AF_INET;
		::inet_aton(ip.constData(), reinterpret_cast<in_addr*>(&_addr.sin_addr));
		_addr.sin_port = ntohs(port);

		_fd = createSokcet();

		_buffer = new char[1024];
	}

	TcpSocket::TcpSocket()
	{
		_buffer = new char[1024];
	}

	TcpSocket::~TcpSocket()
	{
		delete[]_buffer;
	}

	bool TcpSocket::connect()
	{
		if (_isConnect)
		{
			close();
			_fd = createSokcet();
			_isConnect = false;
		}
		int ret = ::connect(_fd, reinterpret_cast<sockaddr*>(&_addr), sizeof(sockaddr));
		if (ret == -1)
		{
			if (errno == EINPROGRESS)
			{
				return false;
			}
			_error.push(Error{ errno,"connect faild" });
			return false;
		}
		setNoBlock(_fd);
		_isConnect = true;
		return true;
	}

	bool TcpSocket::connect(const CMByteArray& ip, u_short port)
	{
		_addr = { 0 };
		_addr.sin_family = AF_INET;
		::inet_aton(ip.constData(), reinterpret_cast<in_addr*>(&_addr.sin_addr));
		_addr.sin_port = ntohs(port);

		return connect();
	}

	bool TcpSocket::isConnected()
	{
		return _isConnect;
	}

	CMByteArray TcpSocket::read(size_t size)
	{
		/*if (size == -1)
		{

		}
		::read(_fd, _buffer,);*/


		return {};
	}

	CMByteArray TcpSocket::readAll()
	{
		CMByteArray data;
		while (_isConnect)
		{
			memset(_buffer, 0, 1024);
			int recvRet = ::recv(_fd, _buffer, 1024, 0);
			if (recvRet == 0)
			{
				_isConnect = false;
				close();
			}
			else if (recvRet < 0)
			{
				if (errno == EWOULDBLOCK)
				{
					break;
				}
				_error.push(Error{ errno ,"read error" });
				_isConnect = false;
				close();
			}
			else
			{
				data.append(_buffer, recvRet);
			}
		}

		return data;
	}

	size_t TcpSocket::send(const CMByteArray& data)
	{
		if (!_isConnect)
		{
			return -1;
		}

		int ret = ::send(_fd, data.constData(), data.size(), 0);
		if (ret == 0)
		{
			_isConnect = false;
			close();
			return 0;
		}
		else if (ret < 0)
		{
			if (errno == EWOULDBLOCK)
			{
				return ret;
			}

			_isConnect = false;
			close();
			return -1;
		}
		return ret;
	}

	TcpServer::TcpServer(const CMByteArray& ip, u_short port)
	{
		_addr = { 0 };
		_addr.sin_family = AF_INET;
		::inet_aton(ip.constData(), reinterpret_cast<in_addr*>(&_addr.sin_addr));
		_addr.sin_port = ntohs(port);

		_fd = createNoBlockSokcet();

		int opt = 1;
		::setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
	}

	TcpServer::~TcpServer()
	{

	}

	bool TcpServer::bind()
	{
		int ret = ::bind(_fd, reinterpret_cast<sockaddr*>(&_addr), sizeof(sockaddr));
		if (ret == -1)
		{
			_error.push(Error{ errno,"bind error" });
			return false;
		}
		return true;
	}

	bool TcpServer::listen()
	{
		int ret = ::listen(_fd, 10240);
		if (ret == -1)
		{
			_error.push(Error{ errno,"listen error" });
			return false;
		}
		return true;
	}

	Reactor::TcpSocketPtr TcpServer::accept()
	{
		int clFd = ::accept(_fd, nullptr, nullptr);
		if (clFd == -1)
		{
			return {};
		}
		setNoBlock(clFd);
		TcpSocketPtr clSocket(new TcpSocket());
		clSocket->_fd = clFd;
		clSocket->_isConnect = true;
		return clSocket;
	}

	EchoTcpServer::EchoTcpServer(const CMByteArray& ip, const std::vector<u_short>& ports)
	{
		for (u_short port : ports)
		{
			TcpServerPtr server = std::make_shared<TcpServer>(ip, port);
			_tcpServers[server->_fd] = server;
		}

		_epollFd = ::epoll_create(1);
	}

	EchoTcpServer::~EchoTcpServer()
	{
		::close(_epollFd);
	}

	void EchoTcpServer::start()
	{
		for (const auto& var : _tcpServers)
		{
			int fd = var.first;
			TcpServerPtr& serverPtr = _tcpServers[fd];
			serverPtr->bind();
			serverPtr->listen();

			epoll_event eEvent{ 0 };
			eEvent.events = EPOLLIN;
			eEvent.data.fd = fd;
			::epoll_ctl(_epollFd, EPOLL_CTL_ADD, fd, &eEvent);
		}
		epoll_event events[1000]{ 0 };
		while (true)
		{
			int readySize = ::epoll_wait(_epollFd, events, 1000, 100);
			for (int i = 0; i < readySize; i++)
			{
				int readyFd = events[i].data.fd;
				if (events[i].events & EPOLLIN)
				{
					if (_tcpServers.find(readyFd) != _tcpServers.end())
					{
						TcpSocketPtr clSocketPtr = _tcpServers[readyFd]->accept();
						if (clSocketPtr != nullptr)
						{
							int clFd = clSocketPtr->_fd;
							epoll_event eEvent{ 0 };
							eEvent.events = EPOLLIN | EPOLLET;
							eEvent.data.fd = clFd;
							::epoll_ctl(_epollFd, EPOLL_CTL_ADD, clFd, &eEvent);
							_clSockets[clFd] = clSocketPtr;
						}
					}
					else
					{
						auto itor = _clSockets.find(readyFd);
						if (itor == _clSockets.end())
						{
							continue;
						}
						TcpSocketPtr clSocketPtr = itor->second;
						CMByteArray msg = clSocketPtr->readAll();
						if (!clSocketPtr->isConnected())
						{
							_clSockets.erase(itor);
							::epoll_ctl(_epollFd, EPOLL_CTL_DEL, readyFd, nullptr);
						}
						else if (!msg.isEmpty())
						{
							msg.prepend("Recv To:");
							clSocketPtr->send(msg);
						}
					}

				}

			}

			if (!_clSockets.empty()
				&& (_clSockets.size() % 5000 == 0))
			{
				std::cout << "----- cl size -----: " << _clSockets.size() << '\n';
			}
		}


	}

	TestTcpClient::TestTcpClient(const CMByteArray& ip, const std::vector<u_short>& ports)
		:_ip(ip), _ports(ports)
	{
		_epollFd = ::epoll_create(1);
	}

	TestTcpClient::~TestTcpClient()
	{
		::close(_epollFd);
	}

	void TestTcpClient::start()
	{
		epoll_event events[1000]{ 0 };
		while (true)
		{
			for (u_short port : _ports)
			{
				TcpSocketPtr clPtr = std::make_shared<TcpSocket>(_ip, port);
				if (clPtr->connect())
				{
					int clFd = clPtr->_fd;
					epoll_event eEvent{ 0 };
					eEvent.events = EPOLLIN | EPOLLET;
					eEvent.data.fd = clFd;
					::epoll_ctl(_epollFd, EPOLL_CTL_ADD, clFd, &eEvent);
					_clSockets[clFd] = clPtr;
				}
			}

			if (!_clSockets.empty()
				&& (_clSockets.size() % 1000 == 0))
			{
				std::cout << "----- cl size -----: " << _clSockets.size() << '\n';
			}

			int readySize = ::epoll_wait(_epollFd, events, 1000, 100);
			for (int i = 0; i < readySize; i++)
			{
				int readyFd = events[i].data.fd;
				if (events[i].events & EPOLLIN)
				{
					TcpSocketPtr clSocketPtr = _clSockets[readyFd];
					std::cout << clSocketPtr->readAll().constData() << '\n';
					if (!clSocketPtr->isConnected())
					{
						_clSockets.erase(readyFd);
						::epoll_ctl(_epollFd, EPOLL_CTL_DEL, readyFd, nullptr);
					}
				}
			}

			//每次100个客户端发消息
			int size = 0;
			for (const auto& var : _clSockets)
			{
				var.second->send(CMByteArray("hello server"));
				if (++size > 100)
				{
					break;
				}
			}

		}
	}

}