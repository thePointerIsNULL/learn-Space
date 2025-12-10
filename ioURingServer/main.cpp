#include <cstdio>
#include <liburing.h>
#include <sys/socket.h>
#include<netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

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
void setReadEvent(io_uring* ring, int fd);
void setSendEvent(io_uring* ring, int fd, char* msg, int msgLen);






int main()
{
	//初始化实例 创建sq与cq
	io_uring ring = {};
	io_uring_queue_init(128, &ring, 0);

	int listenFd = socket(AF_INET, SOCK_STREAM, 0);
	int opt = 1;
	::setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

	sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	//::inet_aton("123.60.94.36", reinterpret_cast<in_addr*>(&addr.sin_addr));
	addr.sin_addr.s_addr = 0;
	addr.sin_port = htons(61972);


	::bind(listenFd, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr_in));
	::listen(listenFd, 1024);


	setAcceptEvent(&ring, listenFd);
	io_uring_submit(&ring);

	__kernel_timespec timeout =
	{
		.tv_sec = 0,
		.tv_nsec = 300 * 1000 * 1000
	};

	bool needSubmit = false;
	while (true)
	{
		needSubmit = false;

		io_uring_cqe* cqeP = nullptr;
		int ret = io_uring_wait_cqes(&ring, &cqeP, 1, &timeout, nullptr);
		//io_uring_peek_batch_cqe
		if (ret < 0)
		{
			continue;
		}
		unsigned int head = 0;
		unsigned int count = 0;
		io_uring_for_each_cqe(&ring, head, cqeP)
		{
			count++;
			int cqeRes = cqeP->res;
			EventContext* context = reinterpret_cast<EventContext*>(cqeP->user_data);
			if (cqeRes < 0)
			{
				printf("retrun error:%d", cqeRes);
				if (context->fd > 0)
					close(context->fd);
				delete context;
				context = nullptr;
				continue;
			}
			switch (context->type)
			{
			case EventType::ACCEPT:
			{
				setAcceptEvent(&ring, listenFd);
				setReadEvent(&ring, cqeRes);
				needSubmit = true;
			}
			break;
			case EventType::RECV:
			{
				if (cqeRes == 0)
				{
					close(context->fd);
					printf("socket closed\n");
					break;
				}
				printf("%s\n", context->buffer);
				setSendEvent(&ring, context->fd, context->buffer, cqeRes);
				needSubmit = true;
			}
			break;
			case EventType::SEND:
			{
				if (cqeRes == 0)
				{
					close(context->fd);
					printf("socket closed\n");
					break;
				}
				setReadEvent(&ring, context->fd);
				needSubmit = true;
			}
			break;
			default:
				break;
			}
			delete context;
			context = nullptr;
		}

		io_uring_cq_advance(&ring, count);

		if (needSubmit)
		{
			io_uring_submit(&ring);
		}
	}
	io_uring_queue_exit(&ring);

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
	context->fd = -1;
	sqe->user_data = reinterpret_cast<unsigned long long>(context);

	io_uring_prep_accept(sqe, fd, nullptr, nullptr, 0);
}
void setReadEvent(io_uring* ring, int fd)
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
	context->type = EventType::RECV;
	context->fd = fd;
	sqe->user_data = reinterpret_cast<unsigned long long>(context);

	io_uring_prep_recv(sqe, fd, context->buffer, 2048, 0);

}
void setSendEvent(io_uring* ring, int fd, char* msg, int msgLen)
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
	context->type = EventType::SEND;
	context->fd = fd;
	memcpy(context->buffer, msg, msgLen);
	sqe->user_data = reinterpret_cast<unsigned long long>(context);

	io_uring_prep_send(sqe, fd, context->buffer, msgLen, 0);
}