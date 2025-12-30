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
	KeyValueList analysis(CMByteArray& msgData,size_t& surplusSize);
private:
	static constexpr char* _command[] = { "Get","Set" ,"Del" ,"Exist" };
};

