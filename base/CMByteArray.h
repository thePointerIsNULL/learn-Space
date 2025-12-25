#pragma once
#include <atomic>
#include <string>
#include <memory>

/*
* 字符数组
* 隐式共享 写时复制 非线程安全
* 2025/10/2
*/
class CMByteArray
{
private:
	class Iterator;
	class IteratorConst;
public:

	CMByteArray()noexcept;
	~CMByteArray()noexcept;
	CMByteArray(const char* str, size_t len)noexcept;
	CMByteArray(const char* str)noexcept;
	CMByteArray(size_t capacity)noexcept;
	CMByteArray(const CMByteArray& other)noexcept;
	CMByteArray(CMByteArray&& other)noexcept;
	CMByteArray& operator=(const CMByteArray& other)noexcept;
	CMByteArray& operator=(CMByteArray&& other)noexcept;

	size_t size()const;
	size_t capacity()const;
	CMByteArray& append(const char* str);
	CMByteArray& append(const char* byte, size_t len);
	CMByteArray& append(const CMByteArray& str);
	CMByteArray& insert(size_t pos, const char* byte, size_t len);
	CMByteArray& insert(size_t pos, const char* str);
	CMByteArray& insert(size_t pos, const CMByteArray& str);
	CMByteArray& prepend(const char* str);
	CMByteArray& prepend(const char* byte, size_t len);
	CMByteArray& prepend(const CMByteArray& str);
	CMByteArray& remove(size_t pos, size_t len = -1);
	void detach();
	void clear();
	void reserve(size_t size);
	void squeeze();
	char front()const;
	char back()const;
	bool isEmpty()const { return !_size; };
	char at(size_t pos)const;
	char& operator[](size_t pos);
	Iterator begin();
	IteratorConst begin()const;
	Iterator end();
	IteratorConst end()const;

	const char* data()const;
	char* data();
	const char* constData()const;
private:
	void resetBuffer();
	void insertElement(size_t pos, const char* dataP, size_t len);
	void reomveElement(size_t pos, size_t len);
	class Iterator :public  std::iterator<std::bidirectional_iterator_tag, char>
	{
	public:
		Iterator(char* p)noexcept :_dataP(p) {};
		Iterator& operator++()
		{
			_dataP++;
			return *this;
		}
		Iterator operator++(int)
		{
			Iterator temp = *this;
			++(*this);
			return temp;
		}
		Iterator& operator--()
		{
			_dataP--;
			return *this;
		}
		Iterator operator--(int)
		{
			Iterator temp = *this;
			--(*this);
			return temp;
		}
		bool operator!=(const Iterator& t)const
		{
			return _dataP != t._dataP;
		}
		bool operator==(const Iterator& t)const
		{
			return  _dataP == t._dataP;
		}
		char& operator*()
		{
			return *_dataP;
		}
		char* operator->()
		{
			return _dataP;
		}
	private:
		friend CMByteArray;
		char* _dataP;
	};
	class IteratorConst :public  std::iterator<std::bidirectional_iterator_tag, char>
	{
	public:
		IteratorConst(char* p)noexcept :_dataP(p) {};
		IteratorConst& operator++()
		{
			_dataP++;
			return *this;
		}
		IteratorConst operator++(int)
		{
			IteratorConst temp = *this;
			++(*this);
			return temp;
		}
		IteratorConst& operator--()
		{
			_dataP--;
			return *this;
		}
		IteratorConst operator--(int)
		{
			IteratorConst temp = *this;
			--(*this);
			return temp;
		}
		bool operator!=(const IteratorConst& t)const
		{
			return _dataP != t._dataP;
		}
		bool operator==(const IteratorConst& t)const
		{
			return  _dataP == t._dataP;
		}
		const char& operator*()
		{
			return *_dataP;
		}
		const char* operator->()
		{
			return _dataP;
		}
	private:
		friend CMByteArray;
		char* _dataP;
	};

private:
	static size_t _defInitcapacity;
	static double _step;

	std::shared_ptr<char> _data;
	size_t _size;
	size_t _capacity;

};

