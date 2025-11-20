#include "net.h"
#include <unistd.h>
#include <rte_branch_prediction.h>

#define SRCMAC "00:0c:29:f9:84:53"

static void getUDPPkt(struct rte_mbuf* mbuf, Parameter* params)
{
	char data[] = "ping";
	int dataLen = strlen(data);
	uint16_t pktSize = sizeof(rte_ether_hdr) + sizeof(rte_ipv4_hdr)
		+ sizeof(rte_udp_hdr) + dataLen;

	char* buffer = rte_pktmbuf_mtod(mbuf, char*);
	//以太网
	rte_ether_hdr* etherHdr = (rte_ether_hdr*)buffer;

	//char desMac[RTE_ETHER_ADDR_LEN] = { 0x20, 0xC1, 0x9B, 0x2F, 0x51, 0x7F };//宿主机MAC，跨网段就是网关MAC
	//char srcMac[RTE_ETHER_ADDR_LEN] = { 0x00, 0x0c, 0x29, 0xf9, 0x84, 0x49 };//DPDK所绑定的网卡的MAC地址

	rte_memcpy(etherHdr->d_addr.addr_bytes, params->desMac, RTE_ETHER_ADDR_LEN);
	rte_memcpy(etherHdr->s_addr.addr_bytes, params->srcMac, RTE_ETHER_ADDR_LEN);
	etherHdr->ether_type = htons(RTE_ETHER_TYPE_IPV4);

	//IP包
	static uint16_t pktCount = 0;
	rte_ipv4_hdr* ipHdr = (rte_ipv4_hdr*)(etherHdr + 1);
	/*inet_pton(AF_INET, "192.168.0.104", &ipHdr->src_addr);
	inet_pton(AF_INET, "192.168.0.102", &ipHdr->dst_addr);*/
	ipHdr->src_addr = params->srcIP;
	ipHdr->dst_addr = params->desIP;

	printf("%u---%u\n", ipHdr->src_addr, ipHdr->src_addr);
	/*ipHdr->src_addr = RTE_IPV4(192, 168, 0, 104);
	ipHdr->dst_addr = RTE_IPV4(192, 168, 0, 102);
	printf("%u---%u\n", ipHdr->src_addr, ipHdr->src_addr);*/

	ipHdr->version_ihl = 0x45;
	ipHdr->type_of_service = 0;
	ipHdr->packet_id = htons(pktCount++);
	//ipHdr->fragment_offset = htons(0x4000);
	ipHdr->fragment_offset = 0;
	ipHdr->time_to_live = 64;
	ipHdr->next_proto_id = IPPROTO_UDP;
	ipHdr->total_length = htons(sizeof(rte_ipv4_hdr)
		+ sizeof(rte_udp_hdr) + dataLen);


	//UDP包
	rte_udp_hdr* udpHdr = (rte_udp_hdr*)(ipHdr + 1);
	udpHdr->dst_port = params->desPort;
	udpHdr->src_port = params->srcPort;
	udpHdr->dgram_len = htons(sizeof(rte_udp_hdr) + dataLen);
	rte_memcpy((char*)(udpHdr + 1), data, dataLen);

	udpHdr->dgram_cksum = 0;
	udpHdr->dgram_cksum = rte_ipv4_udptcp_cksum(ipHdr, udpHdr);


	ipHdr->hdr_checksum = 0;
	ipHdr->hdr_checksum = rte_ipv4_cksum(ipHdr);

	// 设置mbuf属性
	mbuf->pkt_len = pktSize;
	mbuf->data_len = pktSize;

	mbuf->l2_len = sizeof(struct rte_ether_hdr);
	mbuf->l3_len = sizeof(struct rte_ipv4_hdr);


	char srcIPAddr[INET_ADDRSTRLEN] = { 0 };
	char desIPAddr[INET_ADDRSTRLEN] = { 0 };

	struct in_addr srcAddr = { 0 };
	struct in_addr desAddr = { 0 };
	rte_memcpy(&srcAddr.s_addr, &ipHdr->src_addr, 4);
	rte_memcpy(&desAddr.s_addr, &ipHdr->dst_addr, 4);

	inet_ntop(AF_INET, &srcAddr, srcIPAddr, INET_ADDRSTRLEN);
	inet_ntop(AF_INET, &desAddr, desIPAddr, INET_ADDRSTRLEN);

	char srcMacStr[RTE_ETHER_ADDR_FMT_SIZE] = { 0 };
	char desMacStr[RTE_ETHER_ADDR_FMT_SIZE] = { 0 };
	rte_ether_format_addr(srcMacStr, RTE_ETHER_ADDR_FMT_SIZE, &etherHdr->s_addr);
	rte_ether_format_addr(desMacStr, RTE_ETHER_ADDR_FMT_SIZE, &etherHdr->d_addr);

	printf("pkt:infor: ip size: %d udp size: %d pkt size:%d\n"
		, htons(ipHdr->total_length)
		, htons(udpHdr->dgram_len)
		, mbuf->data_len);
	printf("src[%s-%s:%d] ----> des[%s-%s:%d] :%s\n"
		, srcMacStr
		, srcIPAddr
		, htons(udpHdr->src_port)
		, desMacStr
		, desIPAddr
		, htons(udpHdr->dst_port)
		, (char*)(udpHdr + 1));


}

//int getTCPPkt(char** pktBuffer, Parameter* params)
//{
//	char msg[] = "ping";
//
//	int pktSize = sizeof(rte_ether_hdr) + sizeof(rte_ipv4_hdr)
//		+ sizeof(rte_tcp_hdr) + strlen(msg);
//
//
//	return pktSize;
//}


void doRun(DPDKDevContext* dpdk, Parameter* params)
{
	if (dpdk == NULL
		|| params == NULL)
	{
		return;
	}
	printf("doRun\n");


	for (int i = 0; i < params->count; i++)
	{
		struct rte_mbuf* txBuf = rte_pktmbuf_alloc(dpdk->mbuffpool);//从内存池中分配一个mbuf

		if (params->mode == 1)
		{
			//pkSize = getTCPPkt(&pkt, params);
		}
		else
		{
			getUDPPkt(txBuf, params);
		}

		int ret = rte_eth_tx_burst(dpdk->portID, 0, &txBuf, 1);

		printf("rte_eth_tx_burst:%d\n", ret);
		usleep(100000); // 100ms
	}

	printf("finish\n");
}
