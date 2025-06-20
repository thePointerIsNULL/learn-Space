#include "network.h"
#include <chrono>

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

	void CMHostAddress::reverseByteEndian(void* des, ulong len)
	{
		char* desP = reinterpret_cast<char*>(des);
		char tmpValue = 0;
		for (ulong i = 0; i < len / 2; i++)
		{
			tmpValue = desP[i];
			desP[i] = desP[len - 1 - i];
			desP[len - 1 - i] = tmpValue;
		}
	}


	CMHostAddress CMHostAddress::fromHostName(const char* hostName)
	{
		CMHostAddress testAddress(hostName);
		if (!testAddress.isNull())
		{
			return testAddress;
		}

		ADDRINFOA hints{ 0 }, * addrinfor{ 0 };
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		if (::getaddrinfo(hostName, nullptr, &hints, &addrinfor) != 0)
		{
			return CMHostAddress::Null;
		}

		CMHostAddress hostAddr(CMHostAddress::Null);

		for (ADDRINFOA* infor = addrinfor; infor != nullptr; infor = infor->ai_next)
		{
			//sockaddr_in* sockaddr = reinterpret_cast<sockaddr_in*>(infor->ai_addr);
			uint ip = -1;
			std::memcpy(&ip, &infor->ai_addr->sa_data[2], sizeof(ip));

			//ulong ip = sockaddr->sin_addr.s_addr;

			reverseByteEndian(&ip, sizeof(ip));
			hostAddr = CMHostAddress(ip);
			if (!hostAddr.isNull())
			{
				break;
			}
		}

		::freeaddrinfo(addrinfor);

		return hostAddr;
	}

	CMHostAddress::CMHostAddress(CommonHost host)
		:m_ip(-1)
	{
		switch (host)
		{
		case CMCode::CMHostAddress::Null:
			break;
		case CMCode::CMHostAddress::AnyIpv4:
			m_ip = 0;
			break;
		case CMCode::CMHostAddress::LocalHost:
			m_ip = 0x0100007F;
			break;
		default:
			break;
		}
	}

	CMHostAddress::CMHostAddress(const char* ip)
		:m_ip(-1)
	{
		if (inet_pton(AF_INET/*IPv4*/, ip, &m_ip) < 1)
		{
			m_ip = -1;
		}
	}

	CMHostAddress::CMHostAddress(uint ip)
		:m_ip(ip)
	{
		reverseByteEndian(&m_ip, sizeof(m_ip));
	}

	bool CMHostAddress::isNull() const
	{
		return m_ip == -1;
	}

	std::string CMHostAddress::toString()const
	{
		if (isNull())
		{
			return std::string();
		}

		char str[INET_ADDRSTRLEN]{ 0 };
		::InetNtopA(AF_INET, &m_ip, str, INET_ADDRSTRLEN);

		return std::string(str);
	}

	CMTcpServer::CMTcpServer()
		:CMSocket()
	{
		m_socket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
		if (m_socket == INVALID_SOCKET)
		{
			m_error.push("Error create tcpServer socket:" + std::to_string(WSAGetLastError()));
		}
		else
		{
			ulong data = 1;
			::ioctlsocket(m_socket, FIONBIO, &data);
		}
	}

	bool CMTcpServer::bind(const CMHostAddress& address, ushort port)
	{
		sockaddr_in addr{};
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.s_addr = address.ip();

		//inet_pton(AF_INET/*IPv4*/, "127.0.0.1", &addr.sin_addr);

		if (::bind(m_socket, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr)) == SOCKET_ERROR)
		{
			m_error.push("Binding failed:" + std::to_string(::WSAGetLastError()));
			close();
			return false;
		}
		return true;
	}

	bool CMTcpServer::listen()
	{
		int time = 0;
		while (true)
		{
			if (::listen(m_socket, SOMAXCONN) != SOCKET_ERROR)
			{
				return true;
			}

			int error = ::WSAGetLastError();
			if (error == WSAEADDRINUSE
				&& time < 3)
			{
				time++;
				::Sleep(100);
			}
			else
			{
				m_error.push("Listen failed:" + std::to_string(::WSAGetLastError()));
				close();
				return false;
			}

		}

	}

	CMTcpSocketPtr CMTcpServer::accept(ulong timeout)
	{
		auto startingTime = std::chrono::high_resolution_clock::now();

		while (true)
		{
			SOCKET socket = ::accept(m_socket, nullptr, nullptr);
			if (socket != INVALID_SOCKET)
			{
				CMTcpSocketPtr clientSocketPtr = std::make_shared<CMTcpSocket>();
				clientSocketPtr->m_socket = socket;
				return clientSocketPtr;
			}

			int error = ::WSAGetLastError();
			if (error == WSAEWOULDBLOCK)
			{
				auto times = std::chrono::high_resolution_clock::now();

				if (std::chrono::duration_cast<std::chrono::milliseconds>(times - startingTime).count() >= timeout)
				{
					break;
				}
			}
			else
			{
				m_error.push("Accept failed:" + std::to_string(::WSAGetLastError()));
				close();
				break;
			}
			Sleep(1);

		}
		return nullptr;
	}



	CMTcpServer::~CMTcpServer()
	{
		close();
	}

	void CMSocket::setSocketBufferSize(uint size)
	{
		m_initBufferSize = size;
	}

	bool CMSocket::readable() const
	{
		if (!isValidSocket())
		{
			return false;
		}
		char tmpBuffer[1]{ 0 };
		return ::recv(m_socket, tmpBuffer, 1, MSG_PEEK) > 0;
	}

	CMByteArray CMSocket::read(ulong size)
	{
		if (!isValidSocket())
		{
			return {};
		}

		int ret = ::recv(m_socket, m_buffer, size, 0);
		if (ret == 0)
		{
			m_error.push("Socket disconnect:");
			close();
			return {};
		}
		else if (ret < 0)
		{
			int error = ::WSAGetLastError();
			if (error != WSAEWOULDBLOCK)
			{
				m_error.push("Read error:" + std::to_string(error));
				close();
			}

			return {};
		}

		return CMByteArray(m_buffer, ret);
	}

	ulong CMSocket::send(const CMByteArray& byteArray)
	{
		if (!isValidSocket())
		{
			return -1;
		}
		const char* dataBuffer = nullptr;
		uint size = 0;
		byteArray.data(dataBuffer, size);

		int ret = ::send(m_socket, dataBuffer, size, 0);
		if (ret == 0)
		{
			m_error.push("Socket disconnect:");
			close();
			return -1;
		}
		else if (ret < 0)
		{
			int error = ::WSAGetLastError();
			if (error != WSAEWOULDBLOCK)
			{
				m_error.push("Send error:" + std::to_string(error));
				close();
			}
			return -1;
		}
		return ret;
	}

	CMByteArray CMSocket::readAll()
	{
		if (!isValidSocket())
		{
			return {};
		}

		CMByteArray byteArray;
		while (true)
		{
			int ret = ::recv(m_socket, m_buffer, m_bufferSize, 0);
			if (ret == 0)
			{
				m_error.push("Socket disconnect:");
				close();
				break;
			}
			else if (ret < 0)
			{
				int error = ::WSAGetLastError();
				if (error == WSAEWOULDBLOCK)
				{
					break;
				}
				else
				{
					m_error.push("Read error:" + std::to_string(error));
					close();
					break;
				}
			}
			else
			{
				byteArray.append(m_buffer, ret);
			}
		}
		return byteArray;
	}

	ulong CMSocket::sendAll(const CMByteArray& byteArray)
	{
		if (!isValidSocket())
		{
			return -1;
		}
		const char* dataBuffer = nullptr;
		uint size = 0;
		byteArray.data(dataBuffer, size);

		ulong sendSize = 0;

		do
		{
			int ret = ::send(m_socket, &dataBuffer[sendSize], size - sendSize, 0);
			if (ret == 0)
			{
				m_error.push("Socket disconnect:");
				close();
				break;
			}
			else if (ret < 0)
			{
				int error = ::WSAGetLastError();
				if (error == WSAEWOULDBLOCK)
				{
					Sleep(1);
				}
				else
				{
					m_error.push("Send error:" + std::to_string(error));
					close();
					break;
				}
			}
			else
			{
				sendSize += ret;
			}

		} while (sendSize < size);

		return sendSize;
	}

	bool CMSocket::isValidSocket() const
	{
		return m_socket != INVALID_SOCKET;
	}

	void CMSocket::close()
	{
		::closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}

	std::string CMSocket::getLastError()
	{
		if (m_error.empty())
		{
			return {};
		}
		std::string error = m_error.top();
		m_error.pop();
		return error;
	}

	SOCKET CMSocket::getSocket() const
	{
		return m_socket;
	}

	uint CMSocket::m_initBufferSize = 1024 * 4;
	CMSocket::CMSocket()
		:m_bufferSize(m_initBufferSize)
		, m_socket(INVALID_SOCKET)
	{
		m_buffer = new char[m_bufferSize] {0};
	}

	CMSocket::~CMSocket()
	{
		delete[]m_buffer;
		m_buffer = nullptr;
	}

	CMTcpSocket::CMTcpSocket()
	{

	}

	CMTcpSocket::~CMTcpSocket()
	{
		close();
	}

	bool CMTcpSocket::connectToHost(const CMHostAddress& address, ushort port, ulong timeout)
	{
		close();
		m_socket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, 0);
		if (m_socket == SOCKET_ERROR)
		{
			m_error.push("Error create socket:" + std::to_string(WSAGetLastError()));
			return false;
		}
		ulong mode = 1;
		ioctlsocket(m_socket, FIONBIO, &mode);

		sockaddr_in addr{ 0 };
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = address.ip();
		addr.sin_port = htons(port);

		auto startingTime = std::chrono::high_resolution_clock::now();

		int ret = ::connect(m_socket, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr_in));
		if (ret != SOCKET_ERROR)
		{
			return true;
		}
		int error = ::WSAGetLastError();
		if (error != WSAEWOULDBLOCK)
		{
			m_error.push("Error connect:" + std::to_string(error));
			return false;
		}
		char state = 0;
		int size = sizeof(state);

		Sleep(timeout);

		if (getsockopt(m_socket, SOL_SOCKET, SO_ERROR, &state, &size) == 0)
		{
			if (state != 0)
			{
				close();
				return false;
			}
		}
		else
		{
			close();
			m_error.push("Failed to get the socket status:" + std::to_string(::WSAGetLastError()));
			return false;
		}

		return true;

	}

	bool CMTcpSocket::isConnected()
	{
		if (!isValidSocket())
		{
			return false;
		}
		int ret = ::send(m_socket, nullptr, 0, 0);
		if (ret == 0)
		{
			m_error.push("Socket disconnect:");
			close();
			return false;
		}
		else if (ret < 0)
		{
			int error = ::WSAGetLastError();
			if (error != WSAEWOULDBLOCK)
			{
				m_error.push("Socket error:" + std::to_string(error));
				return false;
			}
		}
		return true;
	}





}

