#include <cstdio>
#include <iostream>
#include <sys/socket.h>
#include<netinet/in.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <unordered_set>
#include <list>
#include <unistd.h>
#include <string.h>
//#include "CMByteArray.h"
#include "TcpServer.h"
#include <vector>




int main(int argc, char** argv)
{
	std::vector<u_short> ports;
	u_short port = 8000;
	for (size_t i = 0; i < 1000; i++)
	{
		ports.push_back(port);
		port++;
	}

	if (argc == 2)
	{
		Reactor::TestTcpClient client(CMByteArray(argv[1]), ports);
		client.start();
	}
	else
	{


		Reactor::EchoTcpServer server("0", ports);
		server.start();
	}






	/*Reactor::TcpSocket socket("192.168.0.108", 9000);
	if (socket.connect())
	{
		std::cout << "success\n";
	}*/
	/*Reactor::TcpServer server("0", 9000);

	server.bind();
	server.listen();

	std::vector<Reactor::TcpSocketPtr> clSocket;
	while (true)
	{
		Reactor::TcpSocketPtr sockePtr = server.accept();
		if (sockePtr != nullptr)
		{
			clSocket.push_back(sockePtr);
			std::cout << "success\n";
		}
	}*/

	//CMByteArray aaa;
	//aaa.append("65addzdsggdfdfhjbvnnvnvbn");
	//aaa.prepend("AAA");

	//CMByteArray bbb(aaa);
	//bbb.append("bbb");

	//for (char ch: bbb)
	//{
	//	std::cout << ch << '\n';
	//}
	std::cin.get();
	if (argc < 2)
	{
		std::cout << "parameter erro\n";
		return -1;
	}

	//auto setAdrrReusing = [](int fd)
	//	{
	//		int opt = 1;
	//		if (::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) == -1)
	//		{
	//			std::cout << "setAdrrReusing erro\n";
	//		}
	//	};

	//auto setSocketNonBlock = [](int fd)
	//	{
	//		int optValue = ::fcntl(fd, F_GETFL, 0);
	//		optValue |= O_NONBLOCK;
	//		int ret = ::fcntl(fd, F_SETFL, optValue);
	//		if (ret < 0)
	//		{
	//			std::cout << "setSocketNonBlock erro\n";
	//		}
	//	};

	//short port = static_cast<short>(atoi(agrv[1]));
	//int range = 100;//从port起始递增100个端口

	//int epollFd = epoll_create(1);

	//sockaddr_in  addr{ 0 };
	//addr.sin_family = AF_INET;
	//addr.sin_addr.s_addr = 0;

	//std::unordered_set<int> fdSet;
	//for (size_t i = 0; i < range; i++)
	//{
	//	int fd = ::socket(AF_INET, SOCK_STREAM, 0);
	//	if (fd == -1)
	//	{
	//		std::cout << "Failed to create socket\n";
	//		return -2;
	//	}

	//	addr.sin_port = ntohs(port + i);
	//	if (::bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr_in)) == -1)
	//	{
	//		std::cout << "bind error\n";
	//		return -2;
	//	}

	//	if (::listen(fd, 100) == -1)
	//	{
	//		std::cout << "Failed to listen\n";
	//		return -2;
	//	}

	//	setAdrrReusing(fd);
	//	setSocketNonBlock(fd);

	//	epoll_event eEvent{ 0 };
	//	eEvent.events = EPOLLIN | EPOLLET;
	//	eEvent.data.fd = fd;
	//	epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &eEvent);

	//	fdSet.insert(fd);
	//}

	//int fdSize = range;

	//std::list<int> clFdList;
	//epoll_event* ctlEEvent = new epoll_event[10000];
	//char buffer[1024]{ 0 };
	//while (true)
	//{
	//	int ctlSize = epoll_wait(epollFd, ctlEEvent, fdSize, 100);

	//	for (size_t i = 0; i < ctlSize; i++)
	//	{
	//		int fd = ctlEEvent[i].data.fd;
	//		if (fdSet.find(fd) != fdSet.end())//侦听socket
	//		{
	//			int clFd = ::accept(fd, nullptr, nullptr);
	//			if (clFd == -1)
	//			{
	//				continue;
	//			}
	//			setSocketNonBlock(clFd);

	//			epoll_event eEvent{ 0 };
	//			eEvent.events = EPOLLIN | EPOLLET;
	//			eEvent.data.fd = clFd;
	//			epoll_ctl(epollFd, EPOLL_CTL_ADD, clFd, &eEvent);

	//			clFdList.push_back(clFd);
	//			fdSize++;
	//			std::cout << "current Client:" << clFd << "------------fd size:" << fdSize << '\n';
	//		}
	//		else//通讯socket
	//		{
	//			if (ctlEEvent[i].events & EPOLLIN)
	//			{
	//				int recvRet = ::recv(fd, buffer, 1024, 0);

	//				if (recvRet <= 0)
	//				{
	//					::epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, nullptr);
	//					::close(fd);
	//					clFdList.remove(fd);
	//					fdSize--;
	//					if (recvRet == 0)
	//					{
	//						std::cout << fd << ":disconnect \n";
	//					}
	//					else if (errno == EWOULDBLOCK)
	//					{
	//						std::cout << fd << ":no data \n";
	//					}
	//					else
	//					{
	//						std::cout << fd << ":recv error \n";
	//					}
	//					continue;
	//				}

	//				char sendBuffer[1024]{ 0 };
	//				char headr[] = "recv:";
	//				strcpy(sendBuffer, headr);
	//				strcpy(sendBuffer + strlen(headr), buffer);
	//				::send(fd, sendBuffer, strlen(sendBuffer), 0);
	//			}
	//		}
	//	}
	//}

	//delete[]ctlEEvent;
	return 0;
}