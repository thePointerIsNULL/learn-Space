#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <liburing.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <chrono>
#include <vector>
#include <unordered_map>

struct Config
{
	enum Mode
	{
		Amount,
		Time
	};
	unsigned int ip = 0;
	unsigned short port = 0;

	unsigned int clSize = 0;
	int requestSize = -1;
	int durationTime = -1;//s
	unsigned int msgSize = 64;
	Mode mode;
	//static constexpr char msg[] = "abcdefghigklmnopqrstuvwxyz1234567890abcdefghigklmnopqrstuvwxyz\t";
	char msg[64] = "abcdefghigklmnopqrstuvwxyz1234567890abcdefghigklmnopqrstuvwxyz\t";
	io_uring* ring = nullptr;
};

enum class EventType
{
	UNDEF = 0,
	ACCEPT,
	CONNECT,
	RECV,
	SEND
};

struct EventContext
{
	int key = -1;
	EventType type = EventType::UNDEF;
	int fd = 0;
	char buffer[2048] = {};
};


bool initConfig(int argc, char** argv, Config& config);
void startTest(Config& config);

int main(int argc, char** argv)
{

	if (argc < 6)
	{
		printf("params error\n");
		return -1;
	}
	char* ip = argv[1];
	unsigned short port = atoi(argv[2]);

	Config config;
	::inet_aton(ip, reinterpret_cast<in_addr*>(&config.ip));
	config.port = htons(port);

	if (!initConfig(argc - 2, argv + 2, config))
	{
		return -1;
	}

	std::cout << "ip:" << ip << '\n';
	std::cout << "port:" << port << '\n';
	std::cout << "clients:" << config.clSize << '\n';
	if (config.mode == Config::Amount)
		std::cout << "request size:" << config.requestSize << '\n';
	else
		std::cout << "duration time:" << config.durationTime << '\n';
	std::cout << "message size:" << config.msgSize << '\n' << "------------------------------------\n";

	io_uring ring;
	int ret = io_uring_queue_init(config.clSize, &ring, 0);
	config.ring = &ring;

	startTest(config);

	io_uring_queue_exit(&ring);

	return 0;
}

bool initConfig(int argc, char** argv, Config& config)
{
	while (true)
	{
		int ret = getopt(argc, argv, "c:s:t:b:");
		if (ret == -1)
		{
			break;
		}
		switch (ret)
		{
		case 'c':
			config.clSize = atoi(optarg);
			break;
		case 's':
		{
			if (config.requestSize != -1)
			{
				printf("params repetition\n");
				return false;
			}
			config.requestSize = atoi(optarg);
			config.mode = Config::Amount;
		}
		break;
		case 't':
		{
			if (config.durationTime != -1)
			{
				printf("params repetition\n");
				return false;
			}
			config.durationTime = atoi(optarg);
			config.mode = Config::Time;
		}
		break;
		case 'b':
			config.msgSize = atoi(optarg);
			break;
		default:
			break;
		}
	}
	return true;
}

void setReadEvent(io_uring* ring, EventContext* context, int fd)
{
	if (ring == nullptr)
	{
		return;
	}

	io_uring_sqe* sqe = io_uring_get_sqe(ring);
	if (sqe == nullptr)
	{
		return;
	}
	context->type = EventType::RECV;
	context->fd = fd;
	sqe->user_data = reinterpret_cast<unsigned long long>(context);

	io_uring_prep_recv(sqe, fd, context->buffer, 2048, 0);

}
void setSendEvent(io_uring* ring, EventContext* context, int fd, const char* msg, int msgLen)
{
	if (ring == nullptr)
	{
		return;
	}

	io_uring_sqe* sqe = io_uring_get_sqe(ring);
	if (sqe == nullptr)
	{
		return;
	}
	context->type = EventType::SEND;
	context->fd = fd;
	memcpy(context->buffer, msg, msgLen);
	sqe->user_data = reinterpret_cast<unsigned long long>(context);

	io_uring_prep_send(sqe, fd, context->buffer, msgLen, 0);
}

void startTest(Config& config)
{
	sockaddr_in serverAddr = {};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = config.ip;
	serverAddr.sin_port = config.port;


	std::unordered_map<int, EventContext*> eventContextPool;
	std::vector<bool> poolElState;
	for (int i = 0; i < config.clSize + 10; i++)
	{
		EventContext* event = new EventContext();
		event->key = i;
		eventContextPool.insert({ i,event });
		poolElState.push_back(true);
	}

	auto getEventContext = [&]()->EventContext*
		{
			for (int i = 0; i < poolElState.size(); i++)
			{
				if (poolElState[i])
				{
					poolElState[i] = false;
					return eventContextPool.at(i);
				}
			}
			return nullptr;
		};
	auto putEventContext = [&](EventContext* event)
		{
			if (event != nullptr)
			{
				event->fd = -1;
				event->type = EventType::UNDEF;
				memset(event->buffer, 0, 2048);
				poolElState[event->key] = true;
			}
		};

	auto setConnectEvent = [&](int fd)
		{
			EventContext* context = getEventContext();
			if (context == nullptr)
			{
				std::cout << "Lack of EventContext\n";
				return;
			}
			context->fd = fd;
			context->type = EventType::CONNECT;

			io_uring_sqe* sqe = io_uring_get_sqe(config.ring);
			if (sqe == nullptr)
			{
				return;
			}
			memset(sqe, 0, sizeof(io_uring_sqe));
			sqe->user_data = reinterpret_cast<__u64>(context);
			io_uring_prep_connect(sqe, fd, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(sockaddr_in));
		};

	std::vector<int> clSockets;
	for (int i = 0; i < config.clSize; i++)
	{
		int fd = ::socket(AF_INET, SOCK_STREAM, 0);
		setConnectEvent(fd);
		clSockets.push_back(fd);
	}

	int finishedCount = 0;
	unsigned int overTime = 0;
	auto startTime = std::chrono::steady_clock::now(); //获取某一起点的相对时间，不会受系统时间的调整影响

	auto finished = [&]()
		{
			if (config.mode == Config::Time)
			{
				auto now = std::chrono::steady_clock::now();
				auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime);
				overTime = ms.count() / 1000;
				if (overTime >= config.durationTime)
				{
					return true;
				}
			}
			else
			{
				return finishedCount >= config.requestSize;
			}
			return false;
		};


	io_uring_submit(config.ring);

	std::cout << "-------------------start-------------------\n";

	while (!finished())
	{
		io_uring_cqe* cqesP[1024];
		int ready = io_uring_peek_batch_cqe(config.ring, cqesP, 1024);
		for (size_t i = 0; i < ready; i++)
		{
			io_uring_cqe* cqe = cqesP[i];
			EventContext* eventContext = reinterpret_cast<EventContext*>(cqe->user_data);
			int res = cqe->res;
			if (res < 0)
			{
				std::cout << "res error:" << res << '\n';
				::close(eventContext->fd);
				putEventContext(eventContext);
				continue;
			}
			switch (eventContext->type)
			{
			case EventType::CONNECT:
			{
				EventContext* context = getEventContext();
				setSendEvent(config.ring, context, eventContext->fd, config.msg, strlen(config.msg));
			}
			break;
			case EventType::RECV:
			{
				finishedCount++;
				EventContext* context = getEventContext();
				setSendEvent(config.ring, context, eventContext->fd, config.msg, strlen(config.msg));

			}
			break;
			case EventType::SEND:
			{
				EventContext* context = getEventContext();
				setReadEvent(config.ring, context, eventContext->fd);
			}
			break;
			default:
				break;
			}

			putEventContext(eventContext);

		}
		io_uring_cq_advance(config.ring, ready);
		io_uring_submit(config.ring);
	}

	std::cout << "-------------------end-------------------\n";
	if (overTime == 0)
	{
		std::cout << "faild\n";
		return;
	}

	std::cout << "over time:" << overTime << '\n';
	std::cout << "finished count:" << finishedCount << '\n';
	std::cout << "QPS:" << finishedCount / overTime << '\n';


}
