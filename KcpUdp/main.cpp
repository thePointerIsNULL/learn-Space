#include <cstdio>
#include "ikcp.h"
#include <arpa/inet.h>
#include "trace.h"
#include <string.h>
#include <chrono>
#include <fcntl.h>
#include <unistd.h>

#include "CMByteArray.h"

struct User
{
	unsigned int coveKey = 0;
	sockaddr_in addr;
	int fd = -1;
};

int updSend(const char* buf, int len, struct IKCPCB* kcp, void* user)
{
	User* userP = reinterpret_cast<User*>(user);

	return ::sendto(userP->fd, buf, len, 0, reinterpret_cast<sockaddr*>(&userP->addr), sizeof(sockaddr_in));
}

unsigned int getCurrentTime()
{
	auto now = std::chrono::high_resolution_clock::now();
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
		now.time_since_epoch()
	);
	return ms.count();
}

bool isSameAddr(sockaddr_in* a, sockaddr_in* b)
{
	return a->sin_addr.s_addr == b->sin_addr.s_addr
		&& a->sin_port == b->sin_port;
}

bool vaildRecvLen(int len)
{
	if (len < 0)
	{
		return false;
	}
	else if (len < 24)//kcp头部的大小
	{
		return false;
	}
}

int main(int argc, char** argv)
{
	if (argc < 5)
	{
		TRACE("wrong argc:", argc);
		return -1;
	}
	char* ip = argv[1];
	short port = static_cast<unsigned int>(atoi(argv[2]));
	unsigned int coveKey = static_cast<unsigned int>(atoi(argv[3]));

	CMByteArray msg = argv[4];

	User user;
	user.coveKey = coveKey;

	user.addr.sin_family = AF_INET;
	::inet_aton(ip, &user.addr.sin_addr);
	user.addr.sin_port = htons(port);

	user.fd = ::socket(AF_INET, SOCK_DGRAM, 0);
	int optVal = ::fcntl(user.fd, F_GETFL, 0);
	optVal |= O_NONBLOCK;
	::fcntl(user.fd, F_SETFL, optVal);

	sockaddr_in localAddr{ 0 };
	localAddr.sin_family = AF_INET;
	localAddr.sin_addr.s_addr = 0;
	localAddr.sin_port = htons(port);
	::bind(user.fd, reinterpret_cast<sockaddr*>(&localAddr), sizeof(sockaddr_in));

	ikcpcb* kcpcb = ikcp_create(coveKey, &user);
	kcpcb->output = &updSend;

	ikcp_nodelay(kcpcb, 0, 10, 2, 0);

	char buffer[1024]{ 0 };
	while (true)
	{
		ikcp_update(kcpcb, getCurrentTime());

		CMByteArray udpMsg;
		while (true)
		{
			memset(buffer, 0, 1024);
			sockaddr_in fromAddr{ 0 };
			socklen_t addrLen = sizeof(sockaddr_in);

			int recvRet = ::recvfrom(user.fd, buffer, 1024, 0, reinterpret_cast<sockaddr*>(&fromAddr), &addrLen);
			if (recvRet < 0
				|| !isSameAddr(&fromAddr, &user.addr))
			{
				break;
			}
			udpMsg.append(buffer, recvRet);
		}

		if (!udpMsg.isEmpty())
		{
			ikcp_input(kcpcb, udpMsg.data(), udpMsg.size());
		}

		CMByteArray recvMsg;
		while (true)
		{
			memset(buffer, 0, 1024);
			int recvRet = ikcp_recv(kcpcb, buffer, 1024);
			if (recvRet < 0)
			{
				break;
			}
			recvMsg.append(buffer, recvRet);
		}
		if (!recvMsg.isEmpty())
		{
			std::cout << recvMsg.constData() << '\n';
		}

		ikcp_send(kcpcb, msg.constData(), msg.size());

		usleep(10);
		//ikcp_check(kcpcb, getCurrentTime());
	}

	return 0;
}