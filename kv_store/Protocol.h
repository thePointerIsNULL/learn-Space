#pragma once
#include "base.h"
#include <list>
#include <string>

class ProtocolHelper
{
public:
	using Key = std::string;
	using Value = CMByteArray;
	using KeyValue = std::pair<Key, Value>;
	using KeyValueList = std::list<KeyValue>;

	enum Command
	{
		Get,
		Set,
		Del,
		Exist,
		Count
	};

	//解析数据 surplusSize剩余未解析的长度
	static KeyValueList analysis(CMByteArray& msgData, size_t& dissipativeSize);
private:
	static const char* _command[];;
};

