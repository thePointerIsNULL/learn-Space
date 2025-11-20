#pragma once
#include <rte_tcp.h>

#define TCPCLOSE 0
#define TCPSYN_SENT 1
#define TCPSYN_RECV 2
#define TCPESTABLISHED 3
#define TCPCLOSE_WAIT1 4
#define TCPCLOSE_WAIT 5
#define TCPCLOSE_WAIT2 6
#define TCPTIME_WAIT 7
#define TCPLAST_ACK 8
#define TCPLISTEN 9
#define TCPCLOSING 10


#define WindowsSize  10480
#define BuffSize  20480

#define FDSetSize 20480
#define FDCOUNT  FDSetSize * 8


typedef struct rte_tcp_hdr TCPHDR;

typedef struct AddrGrop
{
	uint32_t srcIp;
	uint32_t desIp;
	uint16_t srcPort;
	uint16_t desPort;
}AddrGrop;

typedef struct TcpControlBlock
{
	AddrGrop addrGrop;

	int state;
	int fd;

	char readWindows[WindowsSize];
	char writeWindows[WindowsSize];

	char sendBuff[BuffSize];
	char readBuff[BuffSize];

}TCB;

typedef struct TCBNode
{
	TCB* tcb;
	TCB* nextTcb;
}TCBNode;

void initTCP();
void closeTCP();


void tcp(uint32_t srcIp, uint32_t desIp, TCPHDR* tcpHdr);

int cmSocket();
void cmClose(int fd);
bool cmBind(int fd, sockaddr_in* addr);
bool cmListen(int fd);
int cmAccept(int fd);
bool cmConnect(int fd, sockaddr_in* addr);

int cmSend(int fd, char* data, int size);
int cmRead(int fd, char* buffer, int size);














