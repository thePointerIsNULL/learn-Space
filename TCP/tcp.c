#include "tcp.h"
#include <unistd.h>
#include <rte_malloc.h>

static TCBNode* TCBList = NULL;
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


void tcp(uint32_t srcIp, uint32_t desIp, TCPHDR* tcpHdr)
{
	AddrGrop addrGrop = { 0 };
	addrGrop.srcIp = srcIp;
	addrGrop.desIp = desIp;
	addrGrop.srcPort = tcpHdr->src_port;
	addrGrop.desPort = tcpHdr->dst_port;

	if (TCBList == NULL)
	{
		TCBList = rte_malloc("tcbList", sizeof(TCBNode), 0);
	}

	TCB* tcb = findTCB(&addrGrop);

	assert(tcb == NULL);


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