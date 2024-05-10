// DataStructure.cpp : 定义控制台应用程序的入口点。
//
#include <iostream>
#include <algorithm>
//#include <QtCore/QVector>
#include "Vector.hpp"
#include <random>
#include <chrono>
#include "List.hpp"
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
	for (; t1Itor != t1.end(); t1Itor++, t2Itor++)
	{
		if (*t1Itor != *t2Itor)
		{
			std::cout << "Value != ; " << '\n';
		}
	}

	std::cout << "Verification complete!" << '\n';

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = end - start;

	std::cout << "Verification time: " << elapsed.count() << std::endl;
}


int main()
{
	using namespace MContainer;


	/*Vector<int> v1;
	QVector<int> v2;*/

	//verify(v1, v2);


	return 0;
}







//类型擦除

//#include <iostream>
//#include <vector>
//#include <memory>
//
//class Area 
//{
//public:
//	template <typename T>
//	void  Add(T* shape)
//	{
//		shape_.emplace_back(new Wrapper(shape));
//	}
//
//	void Print() 
//	{
//		for (auto &&elem : shape_) 
//		{
//			std::cout << elem->GetArea() << "\n";
//		}
//	}
//private:
//	class MyShape
//	{
//	public:
//		virtual double GetArea() const = 0;
//		virtual ~MyShape() {}
//	};
//
//	template<typename T>
//	class Wrapper : public MyShape
//	{
//	public:
//		Wrapper(T *t) : t_(t) {}
//		double GetArea() const 
//		{
//			return t_->GetArea();
//		}
//	private:
//		T *t_ = nullptr;
//	};
//
//	std::vector<MyShape*> shape_;
//};
//
//class MyShape {
//public:
//	virtual double GetArea() const = 0;
//	virtual ~MyShape() {}
//};
//
//class Square
//{
//public:
//	Square(double side) : side_(side) {}
//
//	double GetArea() const 
//	{
//		return side_ * side_;
//	}
//private:
//	double side_;
//};
//
//class Rectangle 
//{
//public:
//	Rectangle(double width, double length) : w_(width), h_(length) {}
//
//	double GetArea() const
//	{
//		return w_ * h_;
//	}
//private:
//	double w_;
//	double h_;
//};
//
//class Circle {
//
//public:
//	Circle(double radius) : radius_(radius) {}
//
//	double GetArea() const 
//	{
//		return 3.14 * radius_ * radius_;
//	}
//private:
//	double radius_;
//};
//
//int main()
//{
//	Square s{ 1.0 };
//	Rectangle r{ 1.0, 2.0 };
//	Circle c{ 3.0 };
//
//
//	Area area;
//	area.Add(&s);
//	area.Add(&r);
//	area.Add(&c);
//
//	area.Print();
//
//	return 0;
//}


