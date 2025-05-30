#pragma once
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <string>
namespace CMCode
{
	using uint = u_int;
	using ushort = u_short;


	class CMByteArray
	{
	public:
		CMByteArray();
		CMByteArray(const char* str)noexcept;
		CMByteArray(const std::string& str)noexcept;
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


		void data(const char* byte, uint len)const;
	private:
		void freeMemory();
		void creatMemory(uint size);
	private:
		char* m_buffer = nullptr;
		uint m_bufferLen = 0;
		uint m_size = 0;
		int* m_useCount = nullptr;
	};

	class CMHostAddress
	{
		enum CommonHost
		{
			Null = 0,
			AnyIpv4,
			LocalHost,
		};
	};

	class CMTcpSocket
	{
	public:
		void connectToHost(const CMHostAddress& address, ushort port);
		bool isConnected();
		uint read();
	private:
		SOCKET m_socket;
	};

	class CPMTcpServer
	{



	};










}