#pragma once
#include <map>
#include <queue>
#include<netinet/in.h>
#include <memory>
#include<arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>

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


	ReactorEventObj() = default;
	virtual ~ReactorEventObj() = 0;
	int getFd()const { return _fd; };
	EventType getEventType()const { return _type; }
	TriggerMode getTriggerMode()const { return _mode; }
	void setMan(ReactorServer* manger) { _manger = manger; };
	ReactorServer* getMan()const { return _manger; };
	void onAccept(int fd);
	void onRecv();
	void onSend();
	virtual void onClose();
	virtual void happenError() = 0;
protected:
	virtual void acceptImp(int fd) {};
	virtual void recvImp() {};
	virtual void sendImp() {};
	virtual void closeImp() {};
	int _fd = -1;
	EventType _type = EventType::None;
	TriggerMode _mode = TriggerMode::ET;
	std::queue<CMByteArray>  _recvQueue;
	CMByteArray  _sendArry;
	ReactorServer* _manger = nullptr;
	char _dataBuffer[_bufferSize] = {};

};
using ReactorEventObjPtr = std::shared_ptr<ReactorEventObj>;

class ReactorServer
{
public:
	static void setNoBlock(int fd);

	ReactorServer(ReactorEventObjPtr serverObj);
	~ReactorServer();
	void doRun();
	void addEventObj(ReactorEventObjPtr objPtr);
	void removeEventObj(ReactorEventObjPtr objPtr);
	void updateEventObj(int fd);
private:
	static constexpr int _eventCount = 1024 * 100;


	ReactorEventObjPtr _serverObjPtr;
	int _epollFd = -1;
	std::map<int, ReactorEventObjPtr> _eventMap;
};

