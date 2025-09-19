#include "DNSServe.h"
#include <random>
#include <sstream>
DNSServe::DNSServe(const std::string& domainName)
	:_domainName(domainName)
{

}

DNSServe::~DNSServe()
{

}

std::vector<DNSServe::Ret> DNSServe::query()
{
	std::mt19937 generator(std::random_device{}());
	std::uniform_int_distribution<short> distribution(1, 5000);
	short id = distribution(generator);
	Header header;
	header.id = id;
	header.flags = 0x0100;
	header.qdcount = 1;

	Question question;
	question.qname = _domainName;
	question.qtype = 1;
	question.qclass = 1;

	std::string msg = header.getHeaderStr();
	msg.append(question.getQuestionStr());

	SOCKET udpSocket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (udpSocket == INVALID_SOCKET)
	{
		return {};
	}

	struct sockaddr_in destAddr { 0 };
	destAddr.sin_family = AF_INET;
	int ret = ::inet_pton(AF_INET, "114.114.114.114", &destAddr.sin_addr);
	destAddr.sin_port = htons(53);

	ret = sendto(udpSocket, msg.data(), msg.size(), 0, reinterpret_cast<sockaddr*>(&destAddr), sizeof(sockaddr));
	if (ret == SOCKET_ERROR)
	{
		int error = WSAGetLastError();
	}

	char reMsg[10240]{ 0 };

	int recvSize = ::recv(udpSocket, reMsg, 10240, 0);

	//判断收到的是否是所请求的DNS
	Header reHeader = Header::getHeaderFromChar(reMsg);

	if (reHeader.id != id
		&& reHeader.flags != 0x0101)
	{
		return {};
	}

	auto answerVec = Answer::getAnswerFromChar(&reMsg[msg.size()], reHeader.ancount);

	std::vector<DNSServe::Ret> retV;
	for (auto var : answerVec)
	{
		Ret dnsRet;
		dnsRet.vaild = true;
		dnsRet.ip = var.rdata;
		retV.push_back(dnsRet);
	}

	return retV;
}

std::string DNSServe::Header::getHeaderStr()
{
	std::string ret;
	char data[12]{ 0 };

	id = htons(id);
	flags = htons(flags);
	qdcount = htons(qdcount);
	ancount = htons(ancount);
	nscount = htons(nscount);
	arcount = htons(arcount);

	memcpy(data, &id, 2);
	memcpy(data + 2, &flags, 2);
	memcpy(data + 4, &qdcount, 2);
	memcpy(data + 6, &ancount, 2);
	memcpy(data + 8, &nscount, 2);
	memcpy(data + 10, &arcount, 2);
	ret.append(data, 12);
	return ret;
}

DNSServe::Header DNSServe::Header::getHeaderFromChar(const char* ch)
{
	Header header;

	memcpy(&header.id, &ch[0], 2);
	memcpy(&header.flags, &ch[2], 2);
	memcpy(&header.qdcount, &ch[4], 2);
	memcpy(&header.ancount, &ch[6], 2);
	memcpy(&header.nscount, &ch[8], 2);
	memcpy(&header.arcount, &ch[10], 2);

	header.id = ntohs(header.id);
	header.flags = ntohs(header.flags);
	header.qdcount = ntohs(header.qdcount);
	header.ancount = ntohs(header.ancount);
	header.nscount = ntohs(header.nscount);
	header.arcount = ntohs(header.arcount);

	return header;
}

std::string DNSServe::Question::getQuestionStr()
{
	std::string ret;
	std::vector<std::string> nameVec;

	int pos = 0;
	for (size_t i = 0; i < qname.size(); i++)
	{
		if (qname[i] == '.')
		{
			nameVec.emplace_back(qname.substr(pos, i - pos));
			pos = i + 1;
		}
	}

	nameVec.emplace_back(qname.substr(pos, qname.size() - pos));


	for (const std::string& str : nameVec)
	{
		char size = static_cast<char>(str.size());
		ret.push_back(size);
		ret.append(str);
	}
	ret.push_back(0);

	qtype = htons(qtype);
	qclass = htons(qclass);

	char* typeC = (char*)(&qtype);
	char* classC = (char*)(&qclass);

	ret.append(typeC, 2);
	ret.append(classC, 2);

	return ret;
}

std::vector<DNSServe::Answer> DNSServe::Answer::getAnswerFromChar(const char* ch, int count)
{
	std::vector<DNSServe::Answer> ret;

	int offset = 0;
	for (int i = 0; i < count; i++)
	{
		DNSServe::Answer answer;
		if ((ch[0] & 0xc0) == 0xc0)//指针地址
		{
			short pointer = *reinterpret_cast<const short*>(&ch[0]);
			pointer = pointer & 0;
			offset += 2;
		}
		else
		{
			//answer.type = ntohs(ch[]);
		}

		answer.type = ntohs(*reinterpret_cast<const short*>(&ch[offset]));
		offset += 2;
		answer.aclass = ntohs(*reinterpret_cast<const short*>(&ch[offset]));
		offset += 2;
		answer.ttl = ntohl(*reinterpret_cast<const int*>(&ch[offset]));
		offset += 4;
		answer.rdataLen = ntohs(*reinterpret_cast<const short*>(&ch[offset]));
		offset += 2;

		char ipStr[1024]{ 0 };
		::inet_ntop(AF_INET, &ch[offset], ipStr, INET_ADDRSTRLEN);
		answer.rdata.append(ipStr);

		offset += answer.rdataLen;

		ret.push_back(answer);
	}



	return ret;
}

void Http::requestHttp(const std::string& host, const std::string& content, std::string& retContent)
{
	addrinfo hints{ 0 };
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	addrinfo* retAddr = nullptr;
	int ret = ::getaddrinfo(host.data(), "http", &hints, &retAddr);

	SOCKET tcpSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (tcpSocket == INVALID_SOCKET)
	{
		::freeaddrinfo(retAddr);
		return;
	}
	sockaddr_in addr{ 0 };
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = reinterpret_cast<sockaddr_in*>(retAddr->ai_addr)->sin_addr.s_addr;
	addr.sin_port = htons(80);
	ret = ::connect(tcpSocket, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr));


	std::ostringstream request;
	request << "GET " + content + " HTTP/1.1\r\n";
	request << "Host: " + host + "\r\n";
	request << "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64)\r\n";
	request << "Accept:text/html, application/xhtml+xml\r\n";
	request << "Accept-Language: en-US, en; q=0.9\r\n";
	request << "Connection:close\r\n";
	request << "\r\n";

	std::string requestStr = request.str();

	ret = ::send(tcpSocket, requestStr.data(), requestStr.size(), 0);

	char buffer[1024]{ 0 };
	while (true)
	{
		memset(buffer, 0, 1024);
		int recvSize = recv(tcpSocket, buffer, 1024, 0);
		if (recvSize > 0)
		{
			retContent.append(buffer, recvSize);
		}
		else
		{
			break;
		}
	}

	::freeaddrinfo(retAddr);

}
