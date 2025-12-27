#include "ReactorServer.h"
#include <sys/epoll.h>
#include <fcntl.h>



ReactorEventObj::~ReactorEventObj()
{

}

void ReactorEventObj::onAccept(int fd)
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
		if (size < 0)
		{
			if (errno != EWOULDBLOCK)
			{
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
	int offset = 0;
	while (!_sendArry.isEmpty())
	{
		int ret = send(_fd, _sendArry.constData() + offset, _sendArry.size() - offset, 0);
		if (ret < 0)
		{
			if (errno != EWOULDBLOCK)
			{
				happenError();
			}
			else
			{
				break;
			}
		}
		offset += ret;
	}
	_sendArry.remove(0, offset);
	sendImp();
}


void ReactorEventObj::onClose()
{
	closeImp();
	close(_fd);
	_fd = -1;
}

void ReactorServer::setNoBlock(int fd)
{
	int optVal = ::fcntl(fd, F_GETFL, 0);
	optVal |= O_NONBLOCK;
	::fcntl(fd, F_SETFL, optVal);
}

ReactorServer::ReactorServer(ReactorEventObjPtr serverObj)
	:_serverObjPtr(serverObj) 
{
	_serverObjPtr->setMan(this);
}

ReactorServer::~ReactorServer()
{

}

void ReactorServer::doRun()
{
	_epollFd = epoll_create(1);
	int listenFD = _serverObjPtr->getFd();
	epoll_event event = {};
	event.data.fd = listenFD;
	event.events = EPOLL_EVENTS::EPOLLIN | EPOLL_EVENTS::EPOLLET;
	epoll_ctl(_epollFd, EPOLL_CTL_ADD, listenFD, &event);

	epoll_event events[_eventCount] = {};

	while (true)
	{
		int ready = epoll_wait(_epollFd, events, _eventCount, 0);
		for (size_t i = 0; i < ready; i++)
		{
			epoll_event* event = &events[i];
			int readyFd = event->data.fd;
			if (readyFd == listenFD)
			{
				int clFd = accept(listenFD, nullptr, nullptr);
				if (clFd != -1)
				{
					_serverObjPtr->onAccept(clFd);
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

void ReactorServer::addEventObj(ReactorEventObjPtr objPtr)
{
	int fd = objPtr->getFd();
	if (_eventMap.find(fd) != _eventMap.end())
	{
		return;
	}
	objPtr->setMan(this);
	_eventMap[fd] = objPtr;

	updateEventObj(fd);
}

void ReactorServer::removeEventObj(ReactorEventObjPtr objPtr)
{
	auto pos = _eventMap.erase(objPtr->getFd());
	if (pos != -1)
	{
		epoll_ctl(_epollFd, EPOLL_CTL_DEL, objPtr->getFd(), nullptr);
	}
}

void ReactorServer::updateEventObj(int fd)
{
	auto itor = _eventMap.find(fd);
	if (itor == _eventMap.end())
	{
		return;
	}
	ReactorEventObjPtr objPtr = itor->second;
	epoll_event event = {};
	event.data.fd = objPtr->getFd();
	uint32_t events = 0;
	if (objPtr->getEventType() == ReactorEventObj::EventType::Accept
		|| objPtr->getEventType() == ReactorEventObj::EventType::Read)
	{
		events = EPOLL_EVENTS::EPOLLIN;
	}
	else if(objPtr->getEventType() == ReactorEventObj::EventType::Write)
	{
		events = EPOLL_EVENTS::EPOLLOUT;
	}
	if (objPtr->getTriggerMode() == ReactorEventObj::TriggerMode::ET)
	{
		events |= EPOLL_EVENTS::EPOLLET;
	}

	event.events = events;
	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, objPtr->getFd(), &event) == -1)
	{
		if (errno == EEXIST)
		{
			epoll_ctl(_epollFd, EPOLL_CTL_MOD, objPtr->getFd(), &event);
		}
	}
}
