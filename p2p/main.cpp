#include <cstdio>
#include <sys/socket.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<string.h>
#include <iostream>
#include <thread>
int main(int argc, char** argv)
{
	if (argc < 4)
	{
		std::cout << "arg error:" << argc << '\n';

		return -1;
	}

	const char* ip = argv[1];
	short localPort = static_cast<short>(atoi(argv[2]));
	short remotePort = static_cast<short>(atoi(argv[3]));

	int fd = ::socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in localAddr{ 0 };
	localAddr.sin_family = AF_INET;
	localAddr.sin_port = ntohs(localPort);
	localAddr.sin_addr.s_addr = 0;

	int ret = ::bind(fd, reinterpret_cast<sockaddr*>(&localAddr), sizeof(sockaddr));
	std::cout << "bind end:" << ret << '\n';

	sockaddr_in remoteAddr{ 0 };
	remoteAddr.sin_family = AF_INET;
	ret = ::inet_aton(ip, &remoteAddr.sin_addr);
	remoteAddr.sin_port = ntohs(remotePort);

	while (true)
	{
		ret = ::connect(fd, reinterpret_cast<sockaddr*>(&remoteAddr), sizeof(sockaddr));
		if (ret < 0)
		{
			continue;
		}
		break;
		std::cout << "connect success" << '\n';
	}


	char msg[10] = "hello";
	ret = ::send(fd, msg, 10, 0);

	char buffer[10]{ 0 };
	while (true)
	{
		ret = ::recv(fd, buffer, 10, 0);
		if (ret > 0)
		{
			std::cout << buffer << '\n';
			::send(fd, msg, 10, 0);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(800));
	}

	std::cout << "END" << '\n';

	return 0;
}