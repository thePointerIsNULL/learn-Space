#include "ReactorServer.h"
#include <sys/epoll.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

void ReactorEventObj::accept(int fd)
{
	ReactorServer::setNoBlock(fd);
	acceptImp(fd);
}

void ReactorEventObj::onRecv()
{
	CMByteArray byteArry;
	while (true)
	{
		memset(_dataBuffer, 0, _bufferSize);
		int size = ::recv(_fd, _dataBuffer, _bufferSize, 0);
		if (size < 0 )
		{
			if (errno != EWOULDBLOCK)
			{
				close(_fd);
				_fd = -1;
				happenError();
			}
			else
			{
				break;
			}
		}

		if (size == 0)
		{
			onClose();
			return;
		}
		byteArry.append(_dataBuffer, size);
	}
	_recvQueue.push(std::move(byteArry));
	recvImp();
}

void ReactorEventObj::onSend()
{
	sendImp();
}


void ReactorServer::setNoBlock(int fd)
{
	int optVal = ::fcntl(fd, F_GETFL, 0);
	optVal |= O_NONBLOCK;
	::fcntl(fd, F_SETFL, optVal);
}

void ReactorServer::doRun()
{
	/*_listenFd = socket(AF_INET, SOCK_STREAM, 0);
	int ret = bind(_listenFd, reinterpret_cast<sockaddr*>(&_serverAddr), sizeof(sockaddr_in));
	ret = listen(_listenFd, 1024);

	int opt = 1;
	::setsockopt(_listenFd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));*/

	_epollFd = epoll_create(1);
	/*epoll_event event = {};
	event.data.fd = _listenFd;
	event.events = EPOLL_EVENTS::EPOLLIN | EPOLL_EVENTS::EPOLLET;
	epoll_ctl(_epollFd, EPOLL_CTL_ADD, _listenFd, &event);*/

	epoll_event events[_eventCount] = {};

	while (true)
	{
		int ready = epoll_wait(_epollFd, events, _eventCount, 0);
		for (size_t i = 0; i < ready; i++)
		{
			epoll_event* event = &events[i];
			int readyFd = event->data.fd;
			if (readyFd == _serverObjPtr->getFd())
			{
				int clFd = accept(_serverObjPtr->getFd(), nullptr, nullptr);
				if (clFd != -1)
				{
					_serverObjPtr->accept(clFd);
				}
				continue;
			}

			ReactorEventObjPtr eventPtr = _eventMap[readyFd];
			if (event->events & EPOLL_EVENTS::EPOLLIN)
			{
				eventPtr->onRecv();
			}
			if (event->events & EPOLL_EVENTS::EPOLLOUT)
			{
				eventPtr->onSend();
			}
		}
	}
}

void ReactorServer::recvFromFd(int fd)
{

}
