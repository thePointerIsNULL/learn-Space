#pragma once
#include "ReactorServer.h"

class KVTcpServer : public ReactorEventObj
{
public:
	KVTcpServer(const CMByteArray& ip, short port);
	~KVTcpServer();

	void happenError();
protected:
	void acceptImp(int fd);

private:
	CMByteArray _ip;
	short _port;
};

class KVTcpClient : public ReactorEventObj
{
public:
	KVTcpClient(int fd, ReactorEventObj::EventType eventType, ReactorEventObj::TriggerMode mode = ReactorEventObj::ET);
	~KVTcpClient();
	void happenError();
protected:
	void recvImp();
	void sendImp();
	void closeImp();
};


