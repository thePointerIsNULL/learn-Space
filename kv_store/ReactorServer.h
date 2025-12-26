#pragma once
#include <map>
#include <queue>
#include<netinet/in.h>
#include <memory>
#include "CMByteArray.h"

class ReactorServer;

class ReactorEventObj
{
public:
	enum EventType
	{
		None = 0,
		Accept = 1,
		Read = 1 << 1,
		Write = 1 << 2
	};
	enum TriggerMode
	{
		LT,
		ET
	};
	static constexpr int _bufferSize = 1024 * 10;


	ReactorEventObj(int fd, EventType type, TriggerMode mode) :_fd(fd), _type(type), _mode(mode) {};
	virtual ~ReactorEventObj() = 0;
	int getFd()const { return _fd; };
	void accept(int fd);
	void onRecv();
	void onSend();
	virtual void onClose();
	virtual void happenError() = 0;
protected:
	virtual void acceptImp(int fd) = 0;
	virtual void recvImp() = 0;
	virtual void sendImp() = 0;
	int _fd = -1;
	EventType _type = EventType::None;
	TriggerMode _mode = TriggerMode::ET;
	std::queue<CMByteArray>  _recvQueue;
	CMByteArray  _sendArry;

	char _dataBuffer[_bufferSize] = {};

};
using ReactorEventObjPtr = std::shared_ptr<ReactorEventObj>;

class ReactorServer
{
public:
	static void setNoBlock(int fd);

	ReactorServer(ReactorEventObjPtr serverObj) :_serverObjPtr(serverObj) {};
	~ReactorServer();
	void doRun();
private:
	void recvFromFd(int fd);
private:
	static constexpr int _eventCount = 1024 * 100;


	ReactorEventObjPtr _serverObjPtr;

	int _epollFd = -1;


	std::map<int, ReactorEventObjPtr> _eventMap;
};

