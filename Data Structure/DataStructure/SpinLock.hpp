#pragma once
#include <atomic>
#include <thread>
class SpinLock
{
public:
	SpinLock();
	~SpinLock() = default;
	void lock();
	void unlock();
private:
	std::atomic_bool m_state;
	static int m_count;
};

int SpinLock::m_count = 30;

SpinLock::SpinLock()
	:m_state(false)
{

}

void SpinLock::lock()
{
	int count = 0;
	while (true)
	{
		bool value = false;
		if (m_state.compare_exchange_strong(value, true, std::memory_order_acquire))
		{
			return;
		}

		count++;
		if (count >= m_count)
		{
			count = 0;
			std::this_thread::yield();//ÈÃ³öCPU
		}
	}
}

void SpinLock::unlock()
{
	m_state.store(false, std::memory_order_release);
}
