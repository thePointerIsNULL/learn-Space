#include <cstdio>
#include <liburing.h>
#include <sys/socket.h>
#include<netinet/in.h>
#include <arpa/inet.h>

enum class EventType
{
	UNDEF = 0,
	ACCEPT,
	RECV,
	SEND
};

struct EventContext
{
	EventType type = EventType::UNDEF;
	int fd = 0;
	char buffer[2048] = {};
};

void setAcceptEvent(io_uring* ring, int fd);
void setReadEvent();
void setWriteEvent();







int main()
{
	//初始化实例 创建sq与cq
	io_uring ring = {};
	io_uring_queue_init(128, &ring, 0);

	int listenFd = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	//::inet_aton("123.60.94.36", reinterpret_cast<in_addr*>(&addr.sin_addr));
	addr.sin_addr.s_addr = 0;
	addr.sin_port = ntohs(61972);


	::bind(listenFd, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr_in));
	::listen(listenFd, 1024);


	setAcceptEvent(&ring, listenFd);
	io_uring_submit(&ring);

	__kernel_timespec timeout =
	{
		.tv_nsec = 100,
		.tv_sec = 0
	};

	bool needSubmit = false;
	while (true)
	{
		needSubmit = false;

		io_uring_cqe* cqeP = nullptr;
		int cqeCount = io_uring_wait_cqes(&ring, &cqeP, 1, &timeout, nullptr);
		for (size_t i = 0; i < cqeCount; i++)
		{
			io_uring_cqe* cqe = (cqeP + i);
			if (cqe == nullptr)
			{
				break;
			}
			EventContext* context = reinterpret_cast<EventContext*>(cqe->user_data);
			switch (context->type)
			{
			case EventType::ACCEPT:
			{
				setAcceptEvent(&ring, listenFd);
				needSubmit = true;
			}
			break;
			case EventType::RECV:
			{

			}
			break;
			case EventType::SEND:
			{

			}
			break;
			default:
				break;
			}
			io_uring_cqe_seen(&ring, cqe);
		}

		if (needSubmit)
		{
			io_uring_submit(&ring);
		}
	}




	printf("%s 向你问好!\n", "ioURingServer");
	return 0;
}


void setAcceptEvent(io_uring* ring, int fd)
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

	EventContext* context = new EventContext();
	context->type = EventType::ACCEPT;
	context->fd = fd;
	sqe->user_data = reinterpret_cast<unsigned long long>(context);

	io_uring_prep_accept(sqe, fd, nullptr, nullptr, 0);
}