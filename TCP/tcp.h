#pragma once
#include <rte_tcp.h>

#define TCPSYN_SENT 1
#define TCPSYN_RECV 1 << 1
#define TCPESTABLISHED 1 << 2
#define TCPCLOSE_WAIT1 1 << 3
#define TCPCLOSE_WAIT 1 << 4
#define TCPCLOSE_WAIT2 1 << 5
#define TCPTIME_WAIT 1 << 6

#define WindowsSize  10480
#define BuffSize  20480


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

void tcp(uint32_t srcIp, uint32_t desIp,TCPHDR * tcpHdr);











