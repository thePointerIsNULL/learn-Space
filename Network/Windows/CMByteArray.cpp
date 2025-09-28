#include "CMByteArray.h"

size_t CMByteArray::_defInitcapacity = 1024;
double CMByteArray::_step = 1.5;


CMByteArray::CMByteArray() noexcept
	:_capacity(_defInitcapacity)
	, _size(0)
{
	resetBuffer();
}

CMByteArray::CMByteArray(const char* str) noexcept
{
	_size = strlen(str);
	_capacity = _size;
	resetBuffer();
}

CMByteArray::CMByteArray(size_t capacity) noexcept
	:_capacity(capacity)
	, _size(0)
{
	resetBuffer();
}

CMByteArray::CMByteArray(const CMByteArray& other) noexcept
{
	_data = other._data;
	_size = other._size;
	_capacity = other._capacity;
}

CMByteArray::CMByteArray(CMByteArray&& other) noexcept
{
	_data = other._data;
	_size = other._size;
	_capacity = other._capacity;

	other._data.reset();
	other._size = 0;
	other._capacity = 0;
}

size_t CMByteArray::size() const
{
	return _size;
}


size_t CMByteArray::capacity() const
{
	return _capacity;
}

CMByteArray& CMByteArray::append(const char* str)
{

}

CMByteArray& CMByteArray::append(const char* byte, size_t len)
{

}

CMByteArray& CMByteArray::append(const CMByteArray& str)
{

}

CMByteArray& CMByteArray::insert(size_t pos, const char* byte, size_t len)
{

}

CMByteArray& CMByteArray::insert(size_t pos, const char* str)
{

}

CMByteArray& CMByteArray::insert(size_t pos, const CMByteArray& str)
{

}

CMByteArray& CMByteArray::prepend(const char* str)
{

}


CMByteArray& CMByteArray::prepend(const char* byte, size_t len)
{

}

CMByteArray& CMByteArray::prepend(const CMByteArray& str)
{

}

void CMByteArray::detach()
{

}

void CMByteArray::clear()
{

}

void CMByteArray::reserve(size_t size)
{

}

void CMByteArray::squeeze()
{

}

char CMByteArray::front() const
{

}

char CMByteArray::back() const
{

}

char CMByteArray::at(size_t pos)
{

}

char CMByteArray::operator[](size_t pos)
{

}

const char* CMByteArray::data() const
{

}

char* CMByteArray::data()
{

}

const char* CMByteArray::constData() const
{

}

CMByteArray& CMByteArray::operator=(CMByteArray&& other) noexcept
{

}

CMByteArray& CMByteArray::operator=(const CMByteArray& other) noexcept
{

}

CMByteArray::~CMByteArray() noexcept
{

}


CMByteArray::CMByteArray(const char* str, size_t len) noexcept
	:_size(len), _capacity(len)
{
	resetBuffer();
	std::memcpy(_data.get(), str, len);

}




void CMByteArray::resetBuffer()
{
	std::shared_ptr<char> buffer(new char[_capacity], std::default_delete<char[]>());
	std::memcpy(buffer.get(), _data.get(), _size);
	_data = buffer;
}

