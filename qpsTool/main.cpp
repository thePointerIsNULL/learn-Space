#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <liburing.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <chrono>

struct Config
{
	enum Mode
	{
		Amount,
		Time
	};
	unsigned int ip = 0;
	unsigned short port = 0;

	unsigned int clSize = 0;
	int requestSize = -1;
	int durationTime = -1;//s
	unsigned int msgSize = 64;
	Mode mode;
	static constexpr char msg[] = "abcdefghigklmnopqrstuvwxyz1234567890abcdefghigklmnopqrstuvwxyz\t";
};



bool initConfig(int argc, char** argv, Config& config);
void startTest(Config& config);

int main(int argc, char** argv)
{
	if (argc < 6)
	{
		printf("params error\n");
		return -1;
	}
	char* ip = argv[1];
	unsigned short port = atoi(argv[2]);

	Config config;
	::inet_aton(ip, reinterpret_cast<in_addr*>(&config.ip));
	config.port = htons(port);

	if (!initConfig(argc - 2, argv + 2, config))
	{
		return -1;
	}

	std::cout << "ip:" << ip << '\n';
	std::cout << "port:" << port << '\n';
	std::cout << "clients:" << config.clSize << '\n';
	if (config.mode == Config::Amount)
		std::cout << "request size:" << config.requestSize << '\n';
	else
		std::cout << "duration time:" << config.durationTime << '\n';
	std::cout << "message size:" << config.msgSize << '\n' << "------------------------------------\n";

	startTest(config);

	return 0;
}

bool initConfig(int argc, char** argv, Config& config)
{
	while (true)
	{
		int ret = getopt(argc, argv, "c:s:t:b:");
		if (ret == -1)
		{
			break;
		}
		switch (ret)
		{
		case 'c':
			config.clSize = atoi(optarg);
			break;
		case 's':
		{
			if (config.requestSize != -1)
			{
				printf("params repetition\n");
				return false;
			}
			config.requestSize = atoi(optarg);
			config.mode = Config::Amount;
		}
		break;
		case 't':
		{
			if (config.durationTime != -1)
			{
				printf("params repetition\n");
				return false;
			}
			config.durationTime = atoi(optarg);
			config.mode = Config::Time;
		}
		break;
		case 'b':
			config.msgSize = atoi(optarg);
			break;
		default:
			break;
		}
	}
	return true;
}

void startTest(Config& config)
{
	int count = 0;
	auto startTime = std::chrono::steady_clock::now(); //获取某一起点的相对时间，不会受系统时间的调整影响
	
	auto finished = [&]()
		{
			if (config.mode == Config::Time)
			{
				auto now = std::chrono::steady_clock::now();
				auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime);

			}
			else
			{

			}
			


			return false;
		};

}
