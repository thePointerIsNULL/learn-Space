#include "tcp.h"
#include <unistd.h>
#include <rte_malloc.h>

static TCBNode* TCBList = NULL;
static char FDSET[FDSetSize];


static int isSameAddr(AddrGrop* a, AddrGrop* b)
{
	return (a->srcIp == b->srcIp && a->desIp == b->desIp && a->srcPort == b->srcPort && a->desPort == b->desPort) ? 0 : -1;
}
static void addTCBNode(TCBNode* node)
{
	if (node == NULL)
	{
		return;
	}

	TCBNode* tail = TCBList;
	while (tail->nextTcb != NULL)
	{
		tail = tail->nextTcb;
	}
	tail->nextTcb = node;
}
static void delTCBNode(TCBNode* node)
{
	if (node == NULL)
	{
		return;
	}

	TCBNode* preNode = TCBList;
	while (preNode->nextTcb != node)
	{
		preNode = preNode->nextTcb;
	}
	preNode->nextTcb = NULL;
	rte_free(node);
}

static int getFD()
{
	for (uint32_t i = 0; i < FDCOUNT; i++)
	{
		uint32_t index = i / 8;
		uint32_t pos = i % 8;
		if (FDSET[index] & (1 << pos) == 0)
		{
			FDSET[index] = FDSET[index] | (1 << pos);
			return i;
		}
	}
}

static void resetFD(int fd)
{
	uint32_t index = fd / 8;
	uint32_t pos = fd % 8;

	if (index >= FDSetSize)
	{
		return;
	}
	FDSET[index] = FDSET[index] ^ (1 << pos);
}


void initTCP()
{
	if (TCBList == NULL)
	{
		TCBList = rte_malloc("tcbList", sizeof(TCBNode), 0);
	}

	memset(FDSET, 0, FDSetSize);
}

void closeTCP()
{


}

void tcp(uint32_t srcIp, uint32_t desIp, TCPHDR* tcpHdr)
{
	AddrGrop addrGrop = { 0 };
	addrGrop.srcIp = srcIp;
	addrGrop.desIp = desIp;
	addrGrop.srcPort = tcpHdr->src_port;
	addrGrop.desPort = tcpHdr->dst_port;

	TCB* tcb = findTCB(&addrGrop);

	if (tcb == NULL)
	{
		return;//没有可通迅的TCB
	}





	/*if (tcb == NULL)
	{
		tcb = rte_malloc("tcb", sizeof(TCB), 0);
		tcb->addrGrop = addrGrop;
		tcb->state = TCPCLOSE;
		tcb->fd
	}*/



}

TCB* findTCB(AddrGrop* addr)
{
	if (addr == NULL)
	{
		return NULL;
	}
	TCBNode* node = TCBList->nextTcb;
	while (node != NULL)
	{
		TCB* tcb = node->tcb;
		if (isSameAddr(&tcb->addrGrop, addr) == 0)
		{
			return tcb;
		}
	}

	return NULL;

}
TCB* findTCB(int fd)
{
	TCBNode* node = TCBList->nextTcb;
	while (node != NULL)
	{
		TCB* tcb = node->tcb;
		if (tcb->fd == fd)
		{
			return tcb;
		}
	}
	return NULL;

}


int cmSocket()
{
	int fd = getFD();
	TCB* tcb = rte_malloc("tcb", sizeof(TCB), 0);
	memset(&tcb->addrGrop, 0, sizeof(AddrGrop));
	tcb->fd = fd;
	tcb->state = TCPCLOSE;

	TCBNode* tcbNode = rte_malloc("tcbNode", sizeof(TCBNode), 0);
	tcbNode->tcb = tcb;

	addTCBNode(tcbNode);

	return fd;
}

void cmClose(int fd)
{
	TCB* tcb = findTCB(fd);
	if (tcb == NULL)
	{
		return;
	}

}



