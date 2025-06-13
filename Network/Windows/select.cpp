#include "network.h"


namespace CMCode
{
	constexpr CMCode::CMSelect::EventType operator|(CMSelect::EventType a, CMSelect::EventType b)
	{
		return static_cast<CMSelect::EventType>(static_cast<int>(a) | static_cast<int>(b));
	}

	constexpr CMCode::CMSelect::EventType operator&(CMSelect::EventType a, CMSelect::EventType b)
	{
		return static_cast<CMSelect::EventType>(static_cast<int>(a) & static_cast<int>(b));
	}

	constexpr CMCode::CMSelect::EventType operator~(CMSelect::EventType a)
	{
		return static_cast<CMSelect::EventType>(~static_cast<int>(a));
	}

	bool CMSelect::doRun(uint* timeoutMicroseconds)
	{
		FD_ZERO(&m_readSet);
		FD_ZERO(&m_writeSet);
		FD_ZERO(&m_exceptSet);

		EventType type = EventType::ReadEvent & EventType::WriteEvent;
		for (const auto& var : m_recordMap)
		{
			CMSocket* socket = var.first;
			EventType type = var.second;
			if ((type & EventType::ReadEvent) != EventType::None)
			{
				FD_SET(socket->getSocket(), &m_readSet);
			}
			if ((type & EventType::WriteEvent) != EventType::None)
			{
				FD_SET(socket->getSocket(), &m_writeSet);
			}
			if ((type & EventType::ExceptEvent) != EventType::None)
			{
				FD_SET(socket->getSocket(), &m_exceptSet);
			}
		}

		void* timeoutP = nullptr;
		timeval timeout{};
		if (timeoutMicroseconds != nullptr)
		{
			timeoutP = &timeout;
			timeout.tv_sec = *timeoutMicroseconds / 1000;
			timeout.tv_usec = *timeoutMicroseconds % 1000;
		}

		int ret = ::select(0, &m_readSet, &m_writeSet, &m_exceptSet, static_cast<timeval*>(timeoutP));
		if (ret == 0)
		{
			return false;
		}
		else if (ret == SOCKET_ERROR)
		{
			std::string errorStr;
			int error = ::WSAGetLastError();
			switch (error)
			{
			case WSAENOTSOCK:
			{
				errorStr = "Has invalid SOCKET";
			}
			break;
			case WSAEINVAL:
			{
				errorStr = "Invalid parameter";
			}
			break;
			default:
				break;
			}
			m_error = std::make_pair(error, errorStr);
			return false;
		}

		return true;

	}

	CMSelect::EventType CMSelect::getSocketType(CMSocket* socket) const
	{
		auto itor = m_recordMap.find(socket);
		if (itor != m_recordMap.end())
		{
			return itor->second;
		}
		return EventType::None;
	}
	void CMSelect::setMonitorMap(const MonitorMap& map)
	{
		m_recordMap = map;
	}

	void CMSelect::updateSocketEvent(CMSocket* socket, EventType eventType)
	{
		if (m_recordMap.find(socket) == m_recordMap.end())
		{
			m_recordMap[socket] = EventType::None;
		}

		m_recordMap[socket] = m_recordMap[socket] | eventType;
	}


	void CMSelect::getResult(MonitorMap& resultMap) const
	{
		for (const auto& var : m_recordMap)
		{
			CMSocket* socket = var.first;
			EventType type = var.second;

			EventType retType = EventType::None;

			if (FD_ISSET(socket->getSocket(), &m_readSet))
			{
				retType = retType | EventType::ReadEvent;
			}
			if (FD_ISSET(socket->getSocket(), &m_writeSet))
			{
				retType = retType | EventType::WriteEvent;
			}
			if (FD_ISSET(socket->getSocket(), &m_exceptSet))
			{
				retType = retType | EventType::ExceptEvent;
			}
			if (retType == EventType::None)
			{
				continue;
			}
			resultMap[socket] = retType;
		}
	}


	std::pair<int, std::string> CMSelect::getError()
	{
		auto ret = std::make_pair(m_error.first, m_error.second);
		m_error.first = 0;
		m_error.second.clear();
		return ret;
	}

	void CMSelect::removeSocket(CMSocket* socket)
	{
		if (m_recordMap.find(socket) != m_recordMap.end())
		{
			m_recordMap.erase(socket);
		}
	}

}

