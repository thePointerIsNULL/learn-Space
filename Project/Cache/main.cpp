#include<iostream>
#include<string>
#include<vector>
#include <random>
#include "LRU.hpp"


void createData(int size, std::vector<std::pair<int, std::string>>& dataVector)
{
	dataVector.resize(size);

	for (size_t i = 0; i < size; i++)
	{
		dataVector.emplace_back(std::make_pair(i, std::to_string(i)));
	}

}



int main(int argc, char** argv)
{
	std::vector<std::pair<int, std::string>> dataVector;
	createData(500, dataVector);


	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, 500);


	int size = 100000;
	LRUCache<int, std::string> lruCahce(size * 0.3);

	bool preheat = true;
	int preheatSize = size * 0.3;
	size += preheatSize;
	int hitSize = 0;
	int missSize = 0;
	int key = 0;
	std::string value;
	for (size_t i = 0; i < size; i++)
	{
		key = dis(gen);
		if (lruCahce.get(key, value))
		{
			if (!preheat)
				hitSize++;
		}
		else
		{
			if (!preheat)
				missSize++;
			auto itor = std::find_if(dataVector.begin(), dataVector.end(), [&key](const std::pair<int, std::string>& var)
				{
					return var.first == key;
				});
			if (itor != dataVector.end())
			{
				lruCahce.put(key, itor->second);
			}
		}
		if (i >= preheatSize)
		{
			preheat = false;
		}
	}

	std::cout << "hit:" << hitSize << '\n';
	std::cout << "miss:" << missSize << '\n';


	return 0;
}