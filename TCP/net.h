#pragma once
#include<rte_eal.h>
#include<rte_ethdev.h>
#include<rte_memcpy.h>
#include <rte_ip.h>
#include <rte_udp.h>
#include<arpa/inet.h>
#include <rte_ether.h>
typedef struct Parameter
{
	int mode;
	char desMac[RTE_ETHER_ADDR_LEN];
	char srcMac[RTE_ETHER_ADDR_LEN];
	uint32_t srcIP;
	uint32_t desIP;

	uint16_t srcPort;
	uint16_t desPort;
	int count;
}Parameter;
typedef struct DPDKDevContext
{
	int portID;//Íø¿¨ID
	struct rte_mempool* mbuffpool;
	int poolSize;

}DPDKDevContext;

typedef struct rte_ether_hdr rte_ether_hdr;
typedef struct rte_ipv4_hdr rte_ipv4_hdr;
typedef struct rte_udp_hdr rte_udp_hdr;
typedef struct rte_tcp_hdr rte_tcp_hdr;

void doRun(DPDKDevContext* dpdk, Parameter* params);