#include "CMByteArray.h"
#include <stdexcept>
size_t CMByteArray::_defInitcapacity = 1024;
double CMByteArray::_step = 1.5;
#define _assert(a) if (!(a))throw std::out_of_range("CMByteArray:Index out of range!");

CMByteArray::CMByteArray() noexcept
	:_capacity(_defInitcapacity)
	, _size(0)
{
	resetBuffer();
}

CMByteArray::CMByteArray(const char* str) noexcept
{
	size_t size = strlen(str);
	_capacity = size;
	_size = 0;
	resetBuffer();
	insertElement(0, str, size);
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
	insertElement(_size, str, strlen(str));
	return *this;
}

CMByteArray& CMByteArray::append(const char* byte, size_t len)
{
	insertElement(_size, byte, len);
	return *this;
}

CMByteArray& CMByteArray::append(const CMByteArray& str)
{
	insertElement(_size, str._data.get(), str._size);
	return *this;
}

CMByteArray& CMByteArray::insert(size_t pos, const char* byte, size_t len)
{
	insertElement(pos, byte, len);
	return *this;
}

CMByteArray& CMByteArray::insert(size_t pos, const char* str)
{
	insertElement(pos, str, strlen(str));
	return *this;
}

CMByteArray& CMByteArray::insert(size_t pos, const CMByteArray& str)
{
	insertElement(pos, str._data.get(), str._size);
	return *this;
}

CMByteArray& CMByteArray::prepend(const char* str)
{
	insertElement(0, str, strlen(str));
	return *this;
}


CMByteArray& CMByteArray::prepend(const char* byte, size_t len)
{
	insertElement(0, byte, len);
	return *this;
}

CMByteArray& CMByteArray::prepend(const CMByteArray& str)
{
	insertElement(0, str._data.get(), str._size);
	return *this;
}

CMByteArray& CMByteArray::remove(size_t pos, size_t len)
{
	reomveElement(pos, len);
	return*this;
}

void CMByteArray::detach()
{
	if (!_data.unique())
	{
		resetBuffer();
	}
}

void CMByteArray::clear()
{
	_size = 0;
	_capacity = _defInitcapacity;
	resetBuffer();
}

void CMByteArray::reserve(size_t size)
{
	_capacity = size;
	_size = 0;
	resetBuffer();
}

void CMByteArray::squeeze()
{
	_capacity = _size;
	resetBuffer();
}

char CMByteArray::front() const
{
	return at(0);
}

char CMByteArray::back() const
{
	return at(_size - 1);
}

char CMByteArray::at(size_t pos)const
{
	_assert(pos < _size);
	return _data.get()[pos];
}

char& CMByteArray::operator[](size_t pos)
{
	_assert(pos < _size);
	return _data.get()[pos];
}

CMByteArray::Iterator CMByteArray::begin()
{
	return Iterator(_data.get());
}

CMByteArray::IteratorConst CMByteArray::begin() const
{
	return IteratorConst(_data.get());
}

CMByteArray::Iterator CMByteArray::end()
{
	return Iterator(_data.get() + _size);
}

CMByteArray::IteratorConst CMByteArray::end() const
{
	return IteratorConst(_data.get() + _size);
}

const char* CMByteArray::data() const
{
	return _data.get();
}

char* CMByteArray::data()
{
	return _data.get();
}

const char* CMByteArray::constData() const
{
	return _data.get();
}

CMByteArray& CMByteArray::operator=(CMByteArray&& other) noexcept
{
	_data = other._data;
	_size = other._size;
	_capacity = other._capacity;

	other._data.reset();
	other._size = 0;
	other._capacity = 0;

	return *this;
}

CMByteArray& CMByteArray::operator=(const CMByteArray& other) noexcept
{
	_data = other._data;
	_size = other._size;
	_capacity = other._capacity;
	return *this;
}

CMByteArray::~CMByteArray() noexcept
{

}


CMByteArray::CMByteArray(const char* str, size_t len) noexcept
	:_size(len), _capacity(len)
{
	resetBuffer();
	insertElement(0, str, len);
}




void CMByteArray::resetBuffer()
{
	std::shared_ptr<char> buffer(new char[_capacity] {0}, std::default_delete<char[]>());
	memcpy(buffer.get(), _data.get(), _size);
	_data = buffer;
}

void CMByteArray::insertElement(size_t pos, const char* dataP, size_t dataLen)
{
	_assert(pos <= _size);
	size_t size = _size + dataLen;
	if (size > _capacity)
	{
		_capacity = static_cast<size_t>((_capacity + dataLen) * _step);
	}
	if (!_data.unique())
	{
		resetBuffer();
	}

	size_t moveESize = _size - pos;

	memcpy(_data.get() + _size, _data.get() + pos, moveESize);
	memcpy(_data.get() + pos, dataP, dataLen);

	_size = size;
}

void CMByteArray::reomveElement(size_t pos, size_t len)
{
	_assert(pos <= _size);
	if (!_data.unique())
	{
		resetBuffer();
	}
	if (_size - pos < len)
	{
		len = _size - pos;
	}
	size_t moveSize = _size - pos - len;
	memcpy(_data.get() + pos, _data.get() + pos + len, moveSize);
	_size -= len;
	memset(_data.get() + _size, 0, _capacity - _size);
}


