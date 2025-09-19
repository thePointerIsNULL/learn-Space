#pragma once
#pragma once
#include <vector>
#include <string>

#define Win
#ifdef Win
#include <WS2tcpip.h>
#endif

#ifdef Linux
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<netdb.h>
#endif // Linux


class DNSServe
{
public:
	struct Ret
	{
		bool vaild = false;
		std::string ip;
	};
	DNSServe(const std::string& domainName);
	~DNSServe();
	std::vector<Ret> query();

private:
	std::string _domainName;

	struct Header
	{
		short id = 0;
		short flags = 0;
		short qdcount = 0;
		short ancount = 0;
		short nscount = 0;
		short arcount = 0;

		std::string getHeaderStr();
		static Header getHeaderFromChar(const char* ch);
	};
	struct Question
	{
		std::string qname;
		short qtype = 0;
		short qclass = 0;
		std::string getQuestionStr();
	};

	struct Answer
	{
		std::string name;
		short type = 0;
		short aclass = 0;
		unsigned int ttl = 0;
		short rdataLen = 0;
		std::string rdata;

		static std::vector<Answer> getAnswerFromChar(const char* ch, int count);

	};
};
class Http
{
public:
	static void requestHttp(const std::string& host, const std::string& content, std::string& ret);
protected:
private:
};

