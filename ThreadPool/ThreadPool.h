#pragma once
#include <queue>
#include <vector>
#include <memory>
#include <mutex>
#include <condition_variable>

class ThreadWork;
class Task;
using TaskPtr = std::shared_ptr<Task>;
using TaskQueue = std::queue<TaskPtr>;

class ThreadPool
{
public:
	ThreadPool(int threadCount = -1);
	~ThreadPool();
	ThreadPool(const ThreadPool&) = delete;
	ThreadPool& operator=(const ThreadPool&) = delete;

	ThreadPool& addTask(const TaskPtr& task);
	ThreadPool& addTask(const std::vector<TaskPtr>& tasks);
private:
	void close();
private:
	TaskQueue _tasks;
	std::vector<std::unique_ptr<ThreadWork>> _works;
	std::mutex _mutex;
	std::condition_variable _variable;
	bool _flag;
};

class ThreadWork
{
public:
	~ThreadWork();
private:
	struct Context
	{
		TaskQueue* taskQueue = nullptr;
		std::condition_variable* variable = nullptr;
		std::mutex* mutex = nullptr;
		bool* flag = nullptr;
	};
	ThreadWork(Context taskQueue);

	void doRun();


	Context _context;
	std::unique_ptr<std::thread> _thread;
	friend ThreadPool;
};


class Task
{
public:
	Task() = default;
	virtual ~Task() = default;
	virtual void doRun() = 0;
	bool finished();
protected:
	std::atomic_bool _ret;
};


