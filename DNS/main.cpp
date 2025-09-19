
#include <iostream>
#include "DNSServe.h"
int main(int argc, char** argv)
{
#ifdef Win
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData) != 0;
#endif // DEBUG

	
	DNSServe serve("www.csdn.net");
	auto ret = serve.query();
	for (auto var : ret)
	{
		if (!var.vaild)
		{
			continue;
		}
		std::cout << var.ip << '\n';
	}

	std::string host("www.csdn.net"), content("/"), retStr;
	Http::requestHttp(host, content, retStr);

	std::cout << retStr << '\n';

#ifdef Win
	WSACleanup();
#endif // Win

	return 0;
}

