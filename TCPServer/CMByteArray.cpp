#include "CMByteArray.h"
#include <CString>
namespace CMCode
{
#define CountSub (*m_useCount)--
#define CountAdd (*m_useCount)++
	double CMByteArray::m_step = 1.5;
	CMByteArray::CMByteArray()noexcept
	{
		m_bufferLen = 0;
		m_buffer = nullptr;
		m_useCount.store(new int(1));
	}

	CMByteArray::CMByteArray(const char* str, uint len)noexcept
	{
		m_bufferLen = len;
		m_buffer = new char[m_bufferLen] {0};
		m_size = m_bufferLen;
		std::memcpy(m_buffer, str, m_bufferLen);
		m_useCount.store(new int(1));
	}

	CMByteArray::CMByteArray(CMByteArray&& other)noexcept
	{
		m_bufferLen = other.m_bufferLen;
		m_size = other.m_size;
		m_buffer = other.m_buffer;
		m_useCount.store(other.m_useCount);
		if (m_useCount != nullptr)
		{
			CountAdd;
		}
	}

	CMByteArray::CMByteArray(const CMByteArray& other)noexcept
	{
		m_bufferLen = other.m_bufferLen;
		m_size = other.m_size;
		m_buffer = other.m_buffer;
		m_useCount.store(other.m_useCount);
		if (m_useCount != nullptr)
		{
			CountAdd;
		}
	}

	CMByteArray::CMByteArray(const char* str) noexcept
		:CMByteArray(str, strlen(str))
	{

	}

	CMByteArray::CMByteArray(uint size) noexcept
	{
		m_bufferLen = size;
		m_buffer = new char[m_bufferLen] {0};
		m_useCount.store(new int(1));
	}

	CMByteArray::~CMByteArray()noexcept
	{
		freeMemory();
	}

	CMByteArray& CMByteArray::operator=(CMByteArray&& other)noexcept
	{
		m_bufferLen = other.m_bufferLen;
		m_size = other.m_size;
		m_buffer = other.m_buffer;
		m_useCount.store(other.m_useCount);
		if (m_useCount != nullptr)
		{
			CountAdd;
		}
		return *this;
	}

	CMByteArray& CMByteArray::operator=(const CMByteArray& other)noexcept
	{
		m_bufferLen = other.m_bufferLen;
		m_size = other.m_size;
		m_buffer = other.m_buffer;
		m_useCount.store(other.m_useCount);
		if (m_useCount != nullptr)
		{
			CountAdd;
		}

		return *this;
	}

	uint CMCode::CMByteArray::size()const
	{
		return m_size;
	}

	CMByteArray& CMByteArray::append(const char* str)
	{
		uint strLen = strlen(str);

		this->append(str, strLen);

		return *this;
	}

	CMByteArray& CMByteArray::append(const CMByteArray& str)
	{
		char const* dataP = nullptr;
		uint dataLen = 0;
		str.data(dataP, dataLen);

		return this->append(dataP, dataLen);
	}

	CMByteArray& CMByteArray::append(const char* byte, uint len)
	{
		if (len == 0)
		{
			return*this;
		}

		uint newSize = len + m_size;
		if (*m_useCount > 1)
		{
			creatMemory(newSize * m_step);
		}

		if (m_bufferLen < newSize)
		{
			newSize = newSize * m_step;
			creatMemory(newSize);
		}

		std::memcpy(m_buffer + m_size, byte, len);
		m_size += len;
		return *this;
	}

	CMByteArray& CMByteArray::insert(uint pos, const char* byte, uint len)
	{
		if (pos > m_size
			|| len == 0)
		{
			return *this;
		}
		if (pos == m_size)
		{
			return this->append(byte, len);
		}

		uint newSize = m_size + len;
		if (*m_useCount > 1)
		{
			creatMemory(newSize * m_step);
		}

		if (m_bufferLen < newSize)
		{
			newSize = newSize - m_bufferLen + newSize;
			creatMemory(newSize);
		}

		; std:memcpy(m_buffer + pos + len, m_buffer + pos, static_cast<size_t>(m_size - pos));
		std::memcpy(m_buffer + pos, byte, len);
		m_size += len;
		return *this;
	}

	CMByteArray& CMByteArray::insert(uint pos, const char* str)
	{
		uint len = strlen(str);
		return this->insert(pos, str, len);
	}

	CMByteArray& CMByteArray::insert(uint pos, const CMByteArray& str)
	{
		const char* data = nullptr;
		uint len = 0;
		str.data(data, len);
		return this->insert(pos, data, len);
	}

	void CMByteArray::data(const char*& byte, uint& len)const
	{
		byte = m_buffer;
		len = m_size;
	}

	void CMByteArray::freeMemory()
	{
		if (m_useCount.load() != nullptr)
		{
			if (*m_useCount == 1)
			{
				delete m_useCount;
				m_useCount = nullptr;

				if (m_buffer != nullptr)
				{
					delete m_buffer;
					m_buffer = nullptr;
				}
			}
			else
			{
				CountSub;
			}
		}
	}

	void CMByteArray::creatMemory(uint size)
	{
		char* buffer = new char[size] {0};
		std::memcpy(buffer, m_buffer, size);
		if (*m_useCount == 1)
		{
			delete m_buffer;
			m_buffer = nullptr;
		}
		else
		{
			CountSub;
		}
		m_buffer = buffer;
		m_bufferLen = size;
	}
}