#include <iostream>
#include "ThreadPool.h"

std::mutex mutex;

class AddTask : public Task
{
public:
	AddTask(int* sum);
	~AddTask();
	void doRun();

	int* _sum;
};

AddTask::AddTask(int* sum)
	:_sum(sum)
{

}

AddTask::~AddTask()
{

}

int main(int argc, char** argv)
{

	ThreadPool threadPool;
	int sum = 0;
	std::vector<std::shared_ptr<Task>> allTask;
	for (size_t i = 0; i < 500000; i++)
	{
		auto taskPtr = std::make_shared<AddTask>(&sum);
		allTask.push_back(taskPtr);
		threadPool.addTask(taskPtr);
	}

	while (!allTask.empty())
	{
		for (int i = allTask.size() - 1; i >= 0; i--)
		{
			if (allTask[i]->finished())
			{
				allTask.erase(allTask.begin() + i);
			}
		}

	}



	std::cout << sum << std::endl;

	return 0;
}





void AddTask::doRun()
{
	std::lock_guard<std::mutex> locker(mutex);
	(*_sum)++;
	_ret.store(true, std::memory_order_release);
}