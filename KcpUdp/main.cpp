#include <cstdio>
#include "ikcp.h"
#include <arpa/inet.h>
#include "trace.h"
#include <string.h>
struct User
{
	unsigned int coveKey = 0;
	sockaddr_in addr;
};

int main(int argc, char** argv)
{
	if (argc < 4)
	{
		TRACE("wrong argc:", argc);
		return -1;
	}
	char* ip = argv[1];
	short port = static_cast<unsigned int>(atoi(argv[2]));
	unsigned int coveKey = static_cast<unsigned int>(atoi(argv[3]));

	User user;
	user.coveKey = coveKey;

	sockaddr_in addr{ 0 };

	memcpy(&user.addr, &addr, sizeof(sockaddr_in));

	ikcp_create(coveKey, &addr);

	while (true)
	{

	}


	printf("%s 向你问好!\n", "KcpUdp");
	return 0;
}