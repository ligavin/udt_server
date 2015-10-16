#include <arpa/inet.h>
#include <netdb.h>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <udt.h>
#include <getopt.h>
#include "wbl_comm.h"
#include "info.h"
#include "comm/tools.h"
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <getopt.h>
#include "comm/threadpool.hpp"
#include "task_client.h"
#include "comm/timer.h"

using namespace wbl::util;
using namespace std;

string g_sIp, g_sPort, g_sPath, g_sCount;
int g_iCount = 16;
int g_iFirst = 0;

void showUsage()
{
	printf ("usage: ./client -i(server ip) -p(server port) -f(path) -c(count of threads)\n"
			"./client -i  -p 8887 -f ./ -c 16"
			"\n");
}

void GetFile()
{
	thread_pool<task_client> tp = thread_pool<task_client>(g_iCount, CLIENT_THREAD_POOL_QUEUE_SIZE);

	timer tm;
	struct timeval start;
	gettimeofday( &start, NULL);

	for(int i = 0; i < 9999; )
	{
		if (g_iFirst == 0)
		{
			tm.start();
		}

		task_client tc;
		tc.init(i, g_sIp, g_sPort, g_sPath, start, &g_iFirst);
		while (false == tp.push(tc))
		{
			usleep(100*1000);
		}

		if (i % 50 == 0)
		{
			printf("use time:%.6f\n", tm.get_time()*1.0/1000000);
		}

		if (i < 8000)
		{
			i += 5;
		}
		else if (i < 9000)
		{
			i += 3;
		}
		else if (i < 9200)
		{
			i += 2;
		}
		else
		{
			i++;
		}

	}

	int i = 0;
	while(i++ < 300)
	{
		printf("use time:%.6f\n", tm.get_time()*1.0/1000000);
		sleep(3);
	}
}

int main(int argc, char* argv[])
{
	int opt = 0;

	while(( opt = getopt( argc, argv, "i:p:f:c:")) != -1) {

		switch(opt) {
			case 'f' : //data
				g_sPath = optarg;
				break;
			case 'p' : //data
				g_sPort = optarg;
				break;

			case 'c' : //data
				g_sCount = optarg;
				break;

			case 'i' : //data
				g_sIp = optarg;
				break;

			default /*? h*/:
				showUsage();
				return 0;
		}
	}

	if (g_sPath.empty() || g_sPort.empty() || g_sIp.empty())
	{
		showUsage();
		return -1;
	}


	if (!g_sCount.empty() && is_digits(g_sCount) )
	{
		g_iCount = wbl::util::strto<int>(g_sCount);
	}

	UDT::startup();
	GetFile();
	UDT::cleanup();
   return 0;
}
