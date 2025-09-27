#pragma once
#include <atomic>
#include <memory>
namespace CMCode
{
	using uint = unsigned int;
	using ushort = unsigned short;
	using ulong = unsigned long;

	//Will not be interrupted by \0
	//The ending does not contain \0
	class CMByteArray
	{
	public:
		CMByteArray()noexcept;
		~CMByteArray()noexcept;
		CMByteArray(const char* str, uint len)noexcept;
		CMByteArray(const char* str)noexcept;
		CMByteArray(uint size)noexcept;
		CMByteArray(const CMByteArray& other)noexcept;
		CMByteArray(CMByteArray&& other)noexcept;
		CMByteArray& operator=(const CMByteArray& other)noexcept;
		CMByteArray& operator=(CMByteArray&& other)noexcept;

		uint size()const;
		CMByteArray& append(const char* str);
		CMByteArray& append(const char* byte, uint len);
		CMByteArray& append(const CMByteArray& str);
		CMByteArray& insert(uint pos, const char* byte, uint len);
		CMByteArray& insert(uint pos, const char* str);
		CMByteArray& insert(uint pos, const CMByteArray& str);
		bool isEmpty()const { return !m_size; };

		void data(const char*& byte, uint& len)const;
	private:
		void freeMemory();
		void creatMemory(uint size);
	private:
		std::shared_ptr<char*> m_buffer;
		uint m_bufferLen = 0;
		uint m_size = 0;
		std::atomic<int*> m_useCount;

		static double m_step;
	};
};

