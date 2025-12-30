#include "Protocol.h"

ProtocolHelper::KeyValueList ProtocolHelper::analysis(CMByteArray& msgData, size_t& surplusSize)
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

	auto getCommand = [](CMByteArray& commandStr)->KeyValue
		{
			CMByteArrayVector arrays = commandStr.split("\r\n");
			if (arrays.empty())
			{
				return {};
			}
			size_t type = 0;
			for (; type < Command::Count; type++)
			{
				if (strcmp(arrays.at(0).constData(), _command[type]) == 0)
				{
					break;
				}
			}
			switch (static_cast<Command>(type))
			{
			default:
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

		

	}
	surplusSize = currentPos;
	return ret;
}
