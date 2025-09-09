#include "ThreadPool.h"
#include <thread>
#include<functional>
ThreadPool::ThreadPool(int threadCount)
	:_flag(false)
{
	if (threadCount == -1)
	{
		threadCount = std::thread::hardware_concurrency();
	}
	_works.resize(threadCount);

	ThreadWork::Context context;
	context.mutex = &_mutex;
	context.variable = &_variable;
	context.taskQueue = &_tasks;
	context.flag = &_flag;
	for (int i = 0; i < threadCount; i++)
	{
		_works[i] = std::unique_ptr<ThreadWork>(new ThreadWork(context));
	}
}

ThreadPool::~ThreadPool()
{
	close();
}

ThreadPool& ThreadPool::addTask(const TaskPtr& task)
{
	_mutex.lock();
	_tasks.push(task);
	_mutex.unlock();
	_variable.notify_one();
	return *this;
}


ThreadPool& ThreadPool::addTask(const std::vector<TaskPtr>& tasks)
{
	_mutex.lock();
	for (size_t i = 0; i < tasks.size(); i++)
	{
		_tasks.push(tasks[i]);
	}
	_mutex.unlock();
	_variable.notify_all();
	return *this;
}

void ThreadPool::close()
{
	_mutex.lock();
	TaskQueue().swap(_tasks);
	_flag = true;
	_variable.notify_all();
	_mutex.unlock();

	for (int i = 0; i < _works.size(); i++)
	{
		_works[i]->_thread->join();
	}

	_works.clear();
}

ThreadWork::ThreadWork(Context context)
	:_context(context)
{
	_thread = std::make_unique<std::thread>(std::bind(&ThreadWork::doRun, this));
}

ThreadWork::~ThreadWork()
{
	if (_thread->joinable())
	{
		_thread->join();
	}
}

void ThreadWork::doRun()
{
	while (true)
	{
		std::unique_lock<std::mutex> mutexLock(*_context.mutex);
		auto fun = [&]()
			{
				return !_context.taskQueue->empty() || *_context.flag;
			};
		_context.variable->wait(mutexLock, fun);

		if (_context.taskQueue->empty()
			&& *_context.flag)
		{
			break;
		}

		TaskPtr taskPtr = std::move(_context.taskQueue->front());
		_context.taskQueue->pop();

		mutexLock.unlock();

		taskPtr->doRun();
	}
}

bool Task::finished()
{
	return _ret.load(std::memory_order_acquire);
}
