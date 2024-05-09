// DataStructure.cpp : 定义控制台应用程序的入口点。
//
#include <iostream>
#include <algorithm>
#include <QtCore/QVector>
#include "Vector.hpp"
#include <random>
#include <chrono>
class A
{
public:
	~A() { std::cout << "~A" << '\n'; }
protected:
private:
};

template <typename T1, typename T2 >
void verify(T1& t1, T2& t2 /*标准容器*/)
{
	auto start = std::chrono::high_resolution_clock::now();


	std::mt19937 rng(std::random_device{}());
	std::uniform_int_distribution<int> dist(0, 0xFFFFF);

	int size = dist(rng);
	for (size_t i = 0; i < size; i++)
	{
		int number = dist(rng);
		if (number % 3 == 0)
		{
			t1.append(number);
			t2.append(number);
		}
		else if (number % 3 == 1
			&& number < t2.size())
		{
			t1.insert(number, number);
			t2.insert(number, number);
		}
		else
		{
			if (number < t2.size())
			{
				t1.removeAt(number);
				t2.removeAt(number);
			}
		}
	}

	if (t1.size() != t2.size())
	{
		std::cout << "size != " << '\n';
		return;
	}

	auto t1Itor = t1.begin();
	auto t2Itor = t2.begin();
	for (;t1Itor != t1.end() ; t1Itor++,t2Itor++)
	{
		if (*t1Itor != *t2Itor)
		{
			std::cout << "Value != ; "<< '\n';
		}
	}

	std::cout << "Verification complete!" << '\n';

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = end - start;

	std::cout << "Verification time: " << elapsed.count()  << std::endl;
}


int main()
{
	using namespace MContainer;


	Vector<int> v1;
	QVector<int> v2;

	verify(v1, v2);


	return 0;
}

