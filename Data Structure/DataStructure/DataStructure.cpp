// DataStructure.cpp : 定义控制台应用程序的入口点。
//
#include <iostream>
#include <vector>
#include "Vector.hpp"

class A
{
public:
	~A() { std::cout << "~A" << '\n'; }
protected:
private:
};


int main()
{
	using namespace MContainer;


	std::vector<int> vec;

	Vector<int>  vector;
	vector.insert(0,1);
	vector.insert(1,2);
	vector.insert(2,3);
	vector.insert(0,4);
	vector.insert(0, 4);
	vector.insert(0, 4);
	vector.insert(0, 4);
	vector.insert(7,1);
	vector.insert(0, 4);
	vector.insert(0, 4);
	vector.insert(0, 4);
	//vector.removeAt(1,2);

	for (size_t i = 0; i < vector.size(); i++)
	{
		std::cout << vector.at(i) << '\n';
	}

    return 0;
}

