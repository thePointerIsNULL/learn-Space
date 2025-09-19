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
#include <arpa/inet.h>

int main(int argc, char** argv)
{
	if (argc < 3)
	{
		std::cout << "parameter erro\n";
		return -1;
	}

	auto setAdrrReusing = [](int fd)
		{
			int opt = 1;
			if (::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) == -1)
			{
				std::cout << "setAdrrReusing erro\n";
			}
		};

	auto setSocketNonBlock = [](int fd)
		{
			int optValue = ::fcntl(fd, F_GETFL, 0);
			optValue |= O_NONBLOCK;
			if (::fcntl(fd, optValue, 0) < 0)
			{
				std::cout << "setSocketNonBlock erro\n";
			}
		};

	short port = atoi(argv[2]);

	sockaddr_in serverAddr{ 0 };
	serverAddr.sin_family = AF_INET;
	::inet_aton(argv[1], &serverAddr.sin_addr);

	char buffer[1024]{ 0 };
	int range = 0;
	while (true)
	{
		int fd = ::socket(AF_INET, SOCK_STREAM, 0);
		if (fd == -1)
		{
			std::cout << "invalid socket\n";
			return -1;
		}

		serverAddr.sin_port = ntohs(port + range);
		//std::cout << "port:"<<port + range<<std::endl;

		range++;
		if (range >= 100)
		{
			range = 0;
		}
		if (::connect(fd, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(sockaddr_in)) == -1)
		{
			std::cout << "connect error:"<<errno<<std::endl;
			return -1;
		}

		std::cout << "connect success:"<<fd << '\n'; 

		// std::string msg = "I am client:" + std::to_string(fd);

		// ::send(fd, msg.data(), msg.length(), 0);

		// int recvRet = ::recv(fd, buffer, 1024, 0);
		// if (recvRet > 0)
		// {
			// std::string recvMsg(buffer, recvRet);
			// std::cout << "connect success:" + recvMsg + "--------------" << fd << std::endl;
		// }
	}

	return 0;
}