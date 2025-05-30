#include "network.h"

namespace CMCode
{
#define CountSub (*m_useCount)--
#define CountAdd (*m_useCount)++

	CMByteArray::CMByteArray()
	{
		m_bufferLen = 10;
		m_buffer = new char[m_bufferLen] {0};
	}

	CMByteArray::CMByteArray(const char* str)noexcept
	{
		m_bufferLen = strlen(str);
		m_buffer = new char[m_bufferLen] {0};
		m_size = m_bufferLen;
		std::memcpy(m_buffer, str, m_bufferLen);
		m_useCount = new int(1);
	}

	CMByteArray::CMByteArray(CMByteArray&& other)noexcept
	{
		m_bufferLen = other.m_bufferLen;
		m_size = other.m_size;
		m_buffer = other.m_buffer;
		m_useCount = other.m_useCount;

		other.freeMemory();
	}

	CMByteArray::CMByteArray(const CMByteArray& other)noexcept
	{
		m_bufferLen = other.m_bufferLen;
		m_size = other.m_size;
		m_buffer = other.m_buffer;
		m_useCount = other.m_useCount;
		if (m_useCount != nullptr)
		{
			CountAdd;
		}
	}

	CMByteArray::CMByteArray(const std::string& str)noexcept
		:CMByteArray(str.c_str())
	{

	}



	CMByteArray& CMByteArray::operator=(CMByteArray&& other)noexcept
	{
		m_bufferLen = other.m_bufferLen;
		m_size = other.m_size;
		m_buffer = other.m_buffer;
		m_useCount = other.m_useCount;

		other.freeMemory();

		return *this;
	}

	CMByteArray& CMByteArray::operator=(const CMByteArray& other)noexcept
	{
		m_bufferLen = other.m_bufferLen;
		m_size = other.m_size;
		m_buffer = other.m_buffer;
		m_useCount = other.m_useCount;
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
		uint newSize = len + m_size;
		if (m_bufferLen < newSize)
		{
			newSize = newSize - m_bufferLen + newSize;
			creatMemory(newSize);
		}

		std::memcpy(m_buffer + m_size, byte, len);
		m_size += len;
		return *this;
	}

	CMByteArray& CMByteArray::insert(uint pos, const char* byte, uint len)
	{
		if (pos > m_size)
		{
			return *this;
		}
		if (pos == m_size)
		{
			return this->append(byte, len);
		}

		uint newSize = m_size + len;
		if (m_bufferLen < newSize)
		{
			newSize = newSize - m_bufferLen + newSize;
			creatMemory(newSize);
		}

	std:memcpy(m_buffer + pos, m_buffer + pos + len, static_cast<size_t>(m_size - pos));
		std::memcpy(m_buffer + pos, byte, len);
		m_size += len;
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

	void CMByteArray::data(const char* byte, uint len)const
	{
		byte = m_buffer;
		len = m_size;
	}

	void CMByteArray::freeMemory()
	{
		if (m_useCount != nullptr)
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

