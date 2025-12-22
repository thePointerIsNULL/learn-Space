#include <iostream>
#include <ucontext.h>
#include <unistd.h>

struct FunCtx
{
	ucontext_t* mainCtx = nullptr;
	ucontext_t* ctx = nullptr;
};

void fun1(FunCtx* funCtx)
{
	std::cout << "start fun1\n";
	swapcontext(funCtx->ctx, funCtx->mainCtx);
	std::cout << "end fun1\n";
}

void fun2(FunCtx* funCtx)
{
	std::cout << "start fun2\n";
	swapcontext(funCtx->ctx, funCtx->mainCtx);
	std::cout << "end fun2\n";
}


int main(int argc, char** argv)
{
	char stack1[2048] = {};
	char stack2[2048] = {};
	ucontext_t ctx[3] = {};

	//
	getcontext(&ctx[1]);
	ctx[1].uc_stack.ss_sp = stack1;
	ctx[1].uc_stack.ss_size = 2048;
	ctx[1].uc_link = &ctx[0];

	getcontext(&ctx[2]);
	ctx[2].uc_stack.ss_sp = stack2;
	ctx[2].uc_stack.ss_size = 2048;
	ctx[2].uc_link = &ctx[0];

	FunCtx funCtx1;
	funCtx1.ctx = &ctx[1];
	funCtx1.mainCtx = &ctx[0];
	makecontext(&ctx[1], (void(*)())fun1, 1, &funCtx1);

	FunCtx funCtx2;
	funCtx2.ctx = &ctx[2];
	funCtx2.mainCtx = &ctx[0];
	makecontext(&ctx[2], (void(*)())fun2, 1, &funCtx2);

	std::cout << "main => fun1\n";
	swapcontext(&ctx[0], &ctx[1]);
	std::cout << "fun1 => main\n";
	swapcontext(&ctx[0], &ctx[1]);

	std::cout << "main => fun2\n";
	swapcontext(&ctx[0], &ctx[2]);
	std::cout << "fun2 => main\n";
	swapcontext(&ctx[0], &ctx[2]);


	return 0;
}