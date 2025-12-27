#include "Network.h"
#include <iostream>

KVTcpServer::KVTcpServer(const CMByteArray& ip, short port)
	:_ip(ip), _port(port)
{
	sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	inet_aton(ip.constData(), reinterpret_cast<in_addr*>(&addr.sin_addr));
	addr.sin_port = htons(port);

	_fd = socket(AF_INET, SOCK_STREAM, 0);
	ReactorServer::setNoBlock(_fd);
	int ret = bind(_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr_in));
	ret = listen(_fd, 1024);
	int opt = 1;
	::setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

}

KVTcpServer::~KVTcpServer()
{
	if (_fd != -1)
	{
		close(_fd);
		_fd = -1;
	}
}

void KVTcpServer::happenError()
{

}

void KVTcpServer::acceptImp(int fd)
{
	ReactorEventObjPtr clPtr = std::make_shared<KVTcpClient>(fd, ReactorEventObj::EventType::Read);
	_manger->addEventObj(clPtr);
}



KVTcpClient::KVTcpClient(int fd, ReactorEventObj::EventType eventType, ReactorEventObj::TriggerMode mode /*= ReactorEventObj::ET*/)
{
	_fd = fd;
	_type = eventType;
	_mode = mode;
}

KVTcpClient::~KVTcpClient()
{
	if (_fd != -1)
	{
		close(_fd);
		_fd = -1;
	}
}

void KVTcpClient::happenError()
{

}

void KVTcpClient::recvImp()
{
	while (!_recvQueue.empty())
	{
		const CMByteArray& data = _recvQueue.front();
		std::cout << data.data() << '\n';
		_recvQueue.pop();
	}
}

void KVTcpClient::sendImp()
{

}

void KVTcpClient::closeImp()
{

}
