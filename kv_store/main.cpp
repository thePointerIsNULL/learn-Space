#include <iostream>
#include "Network.h"



int main(int argc, char** argv)
{
	ReactorEventObjPtr tcpServer = std::make_shared<KVTcpServer>("0", 61972);
	ReactorServer reactorServer(tcpServer);

	reactorServer.doRun();
	return 0;
}



