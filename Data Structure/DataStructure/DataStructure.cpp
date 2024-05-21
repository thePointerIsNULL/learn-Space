// DataStructure.cpp : 定义控制台应用程序的入口点。
//
#include <iostream>
#include <algorithm>
//#include <QtCore/QVector>
#include "Vector.hpp"
#include <random>
#include <chrono>
#include "List.hpp"
#include <QtCore/QList>
#include <QtCore/QFile>
#include <QtCore/QTextStream>

class A
{
public:
	~A() { std::cout << "~A" << '\n'; }
protected:
private:
};

template <typename T1, typename T2 >
void verify(T1& t1, T2& t2 /*标准容器*/, int seed = 0xFFFFFF)
{
	std::cout << " \n---- fun start ---- " << std::endl;

	auto start = std::chrono::high_resolution_clock::now();

	QFile file("D:/verify output.txt");
	file.open(QIODevice::WriteOnly);
	QTextStream stream(&file);
	
	std::mt19937 rng(std::random_device{}());
	std::uniform_int_distribution<int> dist(seed / 2, seed);

	int size = dist(rng);
	for (size_t i = 0; i < size; i++)
	{
		int number = dist(rng);
		if (number % 3 == 0)
		{
			t1.append(number);
			t2.append(number);
			stream << QString("append:%1;\n").arg(QString::number(number));
		}
		else if (number % 3 == 1
			&& number < t2.size())
		{
			t1.insert(number, number);
			t2.insert(number, number);
			stream << QString("insert:%1;size:%2\n").arg(QString::number(number)).arg(QString::number(t1.size()));
		}
		else
		{
			if (number < t2.size())
			{
				t1.removeAt(number);
				t2.removeAt(number);
				stream << QString("removeAt:%1;\n").arg(QString::number(number));
			}
		}
	}
	file.close();
	if (t1.size() != t2.size())
	{
		std::cout << "size != " << '\n';
		return;
	}

	auto t1Itor = t1.begin();
	auto t2Itor = t2.begin();
	for (; t1Itor != t1.end(); t1Itor++, t2Itor++)
	{
		decltype(*t1Itor) value1 = *t1Itor;
		decltype(*t2Itor) value2 = *t2Itor;
		if (value1 != value2)
		{
			std::cout << "Value != ; " << '\n';
		}
	}

	std::cout << "Verification complete! Size:" << size << '\n';

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = end - start;

	std::cout << "Verification time: " << elapsed.count() << std::endl;

	std::cout << " ---- fun end ---- \n" << std::endl;
}


int main()
{
	using namespace MContainer;

	StaticList<int> list;
	list.append(1);
	list.append(2);
	list.append(3);
	list.append(4);
	list.append(5);
	list.append(6);
	list.insert(4, 100);
	list.append(0);

	list.removeAt(1, 2);
	for (size_t i = 0; i < list.size(); i++)
	{
		std::cout << list.at(i) << '\n';

	}


	/*Vector<int> v1, v2;
	v1.append(1);
	v1.append(2);
	v1.append(3);
	v1.append(4);
	v1.insert(4, 5);
	v1.append(0);

	v2 = v1;

	for (size_t i = 0; i < v2.size(); i++)
	{
		std::cout << v2.at(i) << '\n';

	}*/
	/*v1.append(1);
	v1.append(2);
	v1.append(3);
	v1.append(4);
	v1.insert(4, 5);
	v1.append(0);

	v2.append(1);
	v2.append(2);
	v2.append(3);
	v2.append(4);
	v2.insert(4, 5);
	v2.append(0);
	for (size_t i = 0; i < v1.size(); i++)
	{
		std::cout << v1.at(i) << ":" << v2.at(i) << '\n';

	}*/
	

	/*List<int> v1;
	QList<int> v2;
	for (size_t i = 0; i < 5; i++)
	{
		v1.clear();
		v2.clear();
		verify(v1, v2, 500000);
	}
*/
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


