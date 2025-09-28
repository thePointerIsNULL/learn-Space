#pragma once
#include <atomic>
#include <string>
#include <memory>

class CMByteArray;
class ByteBuffer
{
private:
	ByteBuffer() = default;
	~ByteBuffer() = default;

	

	friend CMByteArray;
};



class CMByteArray
{
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
	void detach();
	void clear();
	void reserve(size_t size);
	void squeeze();
	char front()const;
	char back()const;
	bool isEmpty()const { return !_size; };
	char at(size_t pos);
	char operator[](size_t pos);

	const char* data()const;
	char* data();
	const char* constData()const;
private:
	void resetBuffer();
private:
	static size_t _defInitcapacity;
	static double _step;

	std::shared_ptr<char> _data;
	size_t _size;
	size_t _capacity;

};

