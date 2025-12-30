#include "Protocol.h"
#include <netinet/in.h>

const char* ProtocolHelper::_command[] = { "Get", "Set", "Del", "Exist" };
ProtocolHelper::KeyValueList ProtocolHelper::analysis(CMByteArray& msgData, size_t& dissipativeSize)
{
	auto getLine = [](const char* data, size_t dataLen, CMByteArray& line)
		{
			size_t i = 0;
			for (; i < dataLen - 1; i++)
			{
				if (data[i] == '\r'
					&& data[i + 1] == '\n')
				{
					break;
				}
			}
			if (i == dataLen - 1)
			{
				return false;
			}
			line.append(data, i);
			return true;
		};

	auto getCommand = [&](CMByteArray& commandStr)->KeyValue
		{
			CMByteArrayVector arrays = commandStr.split("\r\n");
			if (arrays.empty())
			{
				return {};
			}
			size_t type = 0;
			for (; type < Command::Count; type++)
			{
				char* a = nullptr;
				if (strcmp(arrays.at(0).constData(), ProtocolHelper::_command[type]) == 0)
				{
					break;
				}
			}
			KeyValue keyValue;
			switch (static_cast<Command>(type))
			{
			case Command::Get:
			{
				Key key = arrays.at(1).chop(2).data();
				Value value = arrays.at(1).chop(2).data();
			}
			break;
			case Command::Set:
			{

			}
			break;
			case Command::Del:
			{

			}
			break;
			case Command::Exist:
			{

			}
			break;
			default:
				return {};
				break;
			}
		};

	KeyValueList ret;
	/*
	* 17\r\n
	* Set\r\n
	* Key\r\n
	* Value\r\n
	*/

	size_t currentPos = 0;
	const char* data = msgData.constData();
	size_t size = msgData.size();
	while (currentPos < size)
	{
		CMByteArray commandHeader;
		if (!getLine(data, size, commandHeader))
		{
			break;
		}
		currentPos += commandHeader.size() + 2;// \r\n³¤¶È
		
		data = data + currentPos;
		size -= currentPos;

		int commandLen = atoi(commandHeader.constData());
		if (size < commandLen)
		{
			break;
		}
		CMByteArray command;
		command.append(data, commandLen);
		currentPos += commandLen;

		getCommand(command);

	}
	dissipativeSize = currentPos;
	return ret;
}
