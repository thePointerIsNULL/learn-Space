#pragma once
#include <stack>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <string>
#include <memory>
#include <map>

namespace CMCode
{
	using uint = u_int;
	using ushort = u_short;
	using ulong = u_long;

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
		char* m_buffer = nullptr;
		uint m_bufferLen = 0;
		uint m_size = 0;
		int* m_useCount = nullptr;

		static double m_step;
	};

	//Default create using big-endian format
	//Only support IPv4
	class CMHostAddress
	{
	public:
		enum CommonHost
		{
			Null = 0,
			AnyIpv4,
			LocalHost,
		};
		static void reverseByteEndian(void* des, ulong len);
		static CMHostAddress fromHostName(const char* hostName);

		CMHostAddress(CommonHost host);
		explicit CMHostAddress(const char* ip);
		explicit CMHostAddress(uint ip);

		bool isNull()const;
		std::string toString()const;
		ulong ip()const { return m_ip; }
	private:
		uint m_ip;
	};

	class CMSocket
	{
	public:
		static void setSocketBufferSize(uint size);

		bool readable()const;//Will not change the socket status
		CMByteArray read(ulong size);//Read only once
		ulong send(const CMByteArray& byteArray);//The actual length send may not match the expected length
		CMByteArray readAll();
		ulong sendAll(const CMByteArray& byteArray);
		bool isValidSocket()const;
		void close();
		std::string getLastError();
		SOCKET getSocket()const;
	protected:
		CMSocket();
		virtual ~CMSocket();

		SOCKET m_socket;
		std::stack<std::string> m_error;

		char* m_buffer;
		uint m_bufferSize;

		static uint m_initBufferSize;
	};
	class CMTcpServer;
	class CMTcpSocket final :public CMSocket
	{
	public:
		CMTcpSocket();
		~CMTcpSocket();
		bool connectToHost(const CMHostAddress& address, ushort port, ulong timeout = 0);
		bool isConnected();
	private:
		friend CMTcpServer;
	};
	using CMTcpSocketPtr = std::shared_ptr<CMTcpSocket>;

	class CMTcpServer final : public CMSocket
	{
	public:
		CMTcpServer();
		~CMTcpServer();
		CMTcpServer(const CMTcpServer&) = delete;
		CMTcpServer& operator=(const CMTcpServer&) = delete;

		bool bind(const CMHostAddress& address, ushort port);
		bool listen();
		CMTcpSocketPtr accept(ulong timeout = 0);
	};

	class CMSelect
	{
	public:
		enum class EventType
		{
			None = 0,
			ReadEvent = 1,
			WriteEvent = 1 << 1,
			ExceptEvent = 1 << 2
		};
		friend constexpr CMSelect::EventType operator|(CMSelect::EventType a, CMSelect::EventType b);
		friend constexpr CMSelect::EventType operator&(CMSelect::EventType a, CMSelect::EventType b);
		friend constexpr CMSelect::EventType operator~(CMSelect::EventType a);

		using MonitorMap = std::map<CMSocket*, EventType>;

		bool doRun(uint* timeoutMicroseconds = nullptr);
		void setMonitorMap(const MonitorMap& map);
		void updateSocketEvent(CMSocket* socket, EventType eventType);//Auto join the new socket
		void removeSocket(CMSocket* socket);
		void getResult(MonitorMap& resultMap)const;
		EventType getSocketType(CMSocket* socket)const;
		std::pair<int, std::string> getError();
	protected:

	private:
		MonitorMap  m_recordMap;
		std::pair<int, std::string> m_error;
		fd_set m_readSet;
		fd_set m_writeSet;
		fd_set m_exceptSet;
	};

}