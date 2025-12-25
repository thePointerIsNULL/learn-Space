#define _GNU_SOURCE
#include <dlfcn.h>
#include <iostream>
#include <ucontext.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <string.h>
#include <map>
#include <vector>

static int(*_accept)(int, __SOCKADDR_ARG, socklen_t*);
static ssize_t(*_recv)(int, void*, size_t, int);
static ssize_t(*_send)(int, const void*, size_t, int);


static void initHook()
{
	_accept = (int(*)(int, __SOCKADDR_ARG, socklen_t*))dlsym(RTLD_NEXT, "accept");
	_recv = (ssize_t(*)(int, void*, size_t, int))dlsym(RTLD_NEXT, "recv");
	_send = (ssize_t(*)(int, const void*, size_t, int))dlsym(RTLD_NEXT, "send");
}

int accept(int fd, __SOCKADDR_ARG addr, socklen_t* addrLen);
ssize_t recv(int fd, void* buf, size_t len, int flags);
ssize_t send(int fd, const void* buf, size_t len, int flags);

#define STACKSIZE 1024*128

struct CoroutineCtx
{
	int fd = -1;
	ucontext_t ucp = {};
	char stack[STACKSIZE] = {};
};

struct Scheduler
{
	ucontext_t schedulerUcp = {};
	ucontext_t mainUcp = {};

	char stack[STACKSIZE] = {};
	int listenFd = -1;
	int epollFd = -1;
	std::map<int, CoroutineCtx*> fdCxtMap;
};



void setNoBlock(int fd)
{
	int optVal = ::fcntl(fd, F_GETFL, 0);
	optVal |= O_NONBLOCK;
	::fcntl(fd, F_SETFL, optVal);
}


void initCoroutine(int listenFd);

void scheduler(CoroutineCtx* ctx);


static Scheduler* schedulerP;

static void doRun(int listenFd)
{
	std::vector<int> cltFds;
	char buffer[1024] = {};
	while (true)
	{
		int fd = accept(listenFd, nullptr, nullptr);
		if (fd != -1)
		{
			cltFds.push_back(fd);
		}
		for (int clfd : cltFds)
		{
			memset(buffer, 0, 1024);
			int ret = recv(clfd, buffer, 1024, 0);
			if (ret <= 0)
			{
				close(clfd);
				continue;
			}
			send(clfd, buffer, ret, 0);
		}

	}
}

int main(int argc, char** argv)
{
	initHook();
	int listenFd = socket(AF_INET, SOCK_STREAM, 0);
	setNoBlock(listenFd);
	int opt = 1;
	::setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

	sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = 0;
	addr.sin_port = htons(static_cast<short>(atoi(argv[1])));

	bind(listenFd, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr_in));
	listen(listenFd, 1024);

	initCoroutine(listenFd);

	ucontext_t mainUCP = {};
	swapcontext(&mainUCP, &schedulerP->mainUcp);


	return 0;
}


int accept(int fd, __SOCKADDR_ARG addr, socklen_t* addrLen)
{
	int ret = _accept(fd, addr, addrLen);
	if (ret > 0)
	{
		setNoBlock(ret);
		CoroutineCtx* cCtx = new CoroutineCtx;
		cCtx->fd = ret;
		schedulerP->fdCxtMap[ret] = cCtx;
	}
	return ret;
}
ssize_t recv(int fd, void* buf, size_t len, int flags)
{
	while (true)
	{
		int ret = _recv(fd, buf, len, flags);
		if (ret >= 0)
		{
			return ret;
		}
		if (errno == EWOULDBLOCK)// I/O不可用切换到调度器
		{
			CoroutineCtx* cCtx = schedulerP->fdCxtMap[fd];
			if (cCtx == nullptr)
			{
				return ret;
			}
			epoll_event event = {};
			event.data.fd = fd;
			event.events = EPOLLIN;
			epoll_ctl(schedulerP->epollFd, EPOLL_CTL_ADD, fd, &event);

			/*getcontext(&cCtx->ucp);
			cCtx->ucp.uc_stack.ss_sp = cCtx->stack;
			cCtx->ucp.uc_stack.ss_size = STACKSIZE;
			cCtx->ucp.uc_link = nullptr;
			makecontext(&cCtx->ucp, (void(*)())doRun, 1, schedulerP->listenFd);*/

			swapcontext(&cCtx->ucp, &schedulerP->schedulerUcp);
			int c = 11;
		}
	}
}

ssize_t send(int fd, const void* buf, size_t len, int flags)
{
	while (true)
	{
		int ret = _send(fd, buf, len, flags);
		return ret;
		/*if (ret >= 0)
		{
			return ret;
		}
		if (errno == EWOULDBLOCK)
		{
			CoroutineCtx* cCtx = schedulerP->fdCxtMap[fd];
			swapcontext(cCtx->ctx, schedulerP->schedulerUcp);
		}*/
	}
}

static void scheduler();
void initCoroutine(int listenFd)
{
	schedulerP = new Scheduler();
	schedulerP->listenFd = listenFd;
	schedulerP->epollFd = epoll_create(1);
	getcontext(&schedulerP->schedulerUcp);
	schedulerP->schedulerUcp.uc_stack.ss_sp = schedulerP->stack;
	schedulerP->schedulerUcp.uc_stack.ss_size = STACKSIZE;
	schedulerP->schedulerUcp.uc_link = nullptr;

	makecontext(&schedulerP->schedulerUcp, (void(*)())scheduler, 0);

	getcontext(&schedulerP->mainUcp);
	schedulerP->mainUcp.uc_stack.ss_sp = new char[STACKSIZE];
	schedulerP->mainUcp.uc_stack.ss_size = STACKSIZE;
	schedulerP->mainUcp.uc_link = nullptr;
	makecontext(&schedulerP->mainUcp, (void(*)())doRun, 1, listenFd);

}
void scheduler()
{
	epoll_event events[1024] = {};
	while (true)
	{
		int ready = epoll_wait(schedulerP->epollFd, events, 1024, 0);

		for (int i = 0; i < ready; i++)
		{
			epoll_event* event = &events[i];
			CoroutineCtx* cCtx = schedulerP->fdCxtMap[event->data.fd];
			swapcontext(&schedulerP->schedulerUcp, &cCtx->ucp);
		}
	}
}