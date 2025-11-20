#include <stdio.h>
#include <string.h>

#include"net.h"

void initDPDK(int argc, char** argv, DPDKDevContext* dpdkContex);


int main(int argc, char** argv)
{
	if (argc < 8)
	{
		printf("parameter error\n");
		return -1;
	}

	DPDKDevContext dpdkContext = {};
	initDPDK(argc, argv, &dpdkContext);

	for (int i = 1; i < argc; i++)
	{
		printf("%s\n", argv[i]);
	}

	//转换参数
	Parameter parameter = {};
	if (!strcmp(argv[1], "t"))
	{
		parameter.mode = 1;
	}
	else if (!strcmp(argv[1], "n"))
	{
		parameter.mode = 2;
	}
	else
	{
		printf("Incorrect mode\n");
		return -1;
	}
	//rte_ether_unformat_addr(argv[2], (struct rte_ether_addr*)(&parameter.desMac));
	/*rte_ether_unformat_addr("00:50:56:c0:00:08", (struct rte_ether_addr*)(&parameter.desMac));
	rte_ether_unformat_addr("00:0c:29:f9:84:53", (struct rte_ether_addr*)(&parameter.srcMac));*/

	//20:C1:9B:2F:51:7F
	/*char desMac[RTE_ETHER_ADDR_LEN] = { 0x20, 0xC1, 0x9B, 0x2F, 0x51, 0x7F };
	char srcMac[RTE_ETHER_ADDR_LEN] = { 0x00, 0x0c, 0x29, 0xf9, 0x84, 0x49 };*/

	struct rte_ether_addr mac;
	rte_eth_macaddr_get(dpdkContext.portID, &mac);
	rte_memcpy(parameter.srcMac, mac.addr_bytes, RTE_ETHER_ADDR_LEN);
	rte_ether_unformat_addr(argv[2], (struct rte_ether_addr*)(&parameter.desMac));

	inet_pton(AF_INET, argv[3], &parameter.srcIP);
	inet_pton(AF_INET, argv[4], &parameter.desIP);

	parameter.srcPort = htons((short)atoi(argv[5]));
	parameter.desPort = htons((short)atoi(argv[6]));

	parameter.count = atoi(argv[7]);

	doRun(&dpdkContext, &parameter);

	return 0;
}


void initDPDK(int argc, char** argv, DPDKDevContext* dpdkContex)
{
	if (dpdkContex == NULL)
	{
		return;
	}
	if (rte_eal_init(argc, argv) < 0)//初始化环境
	{
		rte_exit(EXIT_FAILURE, "---rte_eal_init---\n");
	}

	uint16_t nb_sys_ports = rte_eth_dev_count_avail();//获取绑定的网卡
	if (nb_sys_ports == 0)
	{
		rte_exit(EXIT_FAILURE, "---rte_eth_dev_count_avail---\n");
	}
	printf("nb_sys_ports:%d\n", nb_sys_ports);

	int portID = 0;

	int num_rx_queues = 1;
	int num_tx_queues = 1;
	struct rte_eth_conf port_conf_default = {};
	port_conf_default.rxmode.max_rx_pkt_len = RTE_ETHER_MAX_LEN;

	int ret = rte_eth_dev_configure(portID
		, num_rx_queues
		, num_tx_queues
		, &port_conf_default);
	if (ret < 0)
	{
		rte_exit(EXIT_FAILURE, "---rte_eth_dev_configure---\n");
	}

	dpdkContex->poolSize = 4096;

	//创建m_bufferPool
	dpdkContex->mbuffpool = rte_pktmbuf_pool_create("mBuff"
		, dpdkContex->poolSize
		, 0
		, 0
		, RTE_MBUF_DEFAULT_BUF_SIZE
		, rte_socket_id()/*返回当前线程对应的NUMA ID 将buffer分配在对应的NUMA上CPU的访问会更快*/);
	if (dpdkContex->mbuffpool == NULL)
	{
		rte_exit(EXIT_FAILURE, "---rte_pktmbuf_pool_create---\n");
	}

	int devNUMAID = rte_eth_dev_socket_id(portID);/*返回当前网卡的所属的NUMA ID，将网卡与rx queue分配在同一NUMA提高性能*/

	//配置rx队列 
	ret = rte_eth_rx_queue_setup(portID
		, 0
		, 128
		, devNUMAID
		, NULL
		, dpdkContex->mbuffpool);
	if (ret < 0)
	{
		rte_exit(EXIT_FAILURE, "---rte_eth_rx_queue_setup---\n");
	}
	//struct rte_eth_txconf txconf = {};
	//配置tx队列
	ret = rte_eth_tx_queue_setup(portID
		, 0
		, 512
		, devNUMAID
		, NULL);
	if (ret < 0)
	{
		rte_exit(EXIT_FAILURE, "---rte_eth_tx_queue_setup---\n");
	}

	ret = rte_eth_dev_start(portID);
	if (ret < 0)
	{
		rte_exit(EXIT_FAILURE, "---rte_eth_dev_start---\n");
	}
}
