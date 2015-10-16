#include <cstdlib>
#include <netdb.h>
#include <fstream>
#include <iostream>
#include <cstring>
#include <getopt.h>
#include "wbl_comm.h"
#include "comm/tools.h"
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <getopt.h>
#include "comm/threadpool.hpp"
#include <stdlib.h>
#include "task.h"
#include <pthread.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include "timer.h"

using namespace std;
using namespace wbl::util;

int g_iCount = 16;
string g_sPort, g_sPath, g_sCount;
int g_FileIndex = 0;
char g_szFile[FILE_NUM][FILE_NAME_SIZE];

void* sendfile(void*);

void showUsage()
{
	printf ("usage: ./server -p(port) -f(path) -c(count of threads)\n"
			"./server -p 8887 -f /data/jowu/tegcode/data -c 16"
			"\n");
}

int filter_fn(const struct dirent * ent)
{
	if(ent->d_type != 8)
		return 0;

	if ( strcmp(".", ent->d_name) == 0 || strcmp("..", ent->d_name) == 0 )
		return 0;

	return 1;
}

void *GetFileList(void *p)
{
	//cout << "start read file" << endl;

	int file_dir_index = 0;
	g_FileIndex = 0;

	while(true)
	{
		while(true)
		{
			char file_dir[256];
			snprintf(file_dir, sizeof(file_dir), "%s/%02d/", g_sPath.c_str(), file_dir_index);

			int n = 0;
			struct dirent **namelist; // struct dirent * namelist[];

			n = scandir(file_dir, &namelist, filter_fn, alphasort);

			int iStart = g_FileIndex % 100;
			//cout << "dir:" << file_dir << ",start:" << iStart <<endl;

			if (iStart == n)//没有增加新数据
				break;

			for (int i = iStart; i < n; ++i)
			{
				if (true != AddFile((char*)g_szFile, g_FileIndex, namelist[i]->d_name) )
					break;
			}

			if (n > 0)
			{
				for (int i = 0; i < n; ++i)
					free(namelist[i]);
				free(namelist);
			}
			//cout << "n:" <<n << endl;

			if (n >= 100)
			{//如果该文件夹下100个文件都读到了。
				file_dir_index++;
			}

			if (n < 100)
			{
				//cout << "get file num:" << g_FileIndex << endl;
				break;
			}

			//cout << "file dir index:" << file_dir_index << endl;

			if (file_dir_index >= 100)
				break;
		}

		//cout << "get file :" << g_FileIndex << endl;
		if (g_FileIndex >= 10000)
		{
			cout << "get all file" << endl;
			return NULL;
		}
		usleep(100);
	}

	return NULL;
}

void server(UDTSOCKET serv)
{
	pthread_t tid;;
	pthread_create(&tid, NULL, GetFileList, NULL);
	pthread_detach(tid);



	thread_pool<task> tp = thread_pool<task>(g_iCount, THREAD_POOL_QUEUE_SIZE);
	sockaddr_storage clientaddr;
	int addrlen = sizeof(clientaddr);

	UDTSOCKET fhandle;

	timer tm;

	int i = 0;

	while (true)
	{
		if (UDT::INVALID_SOCK == (fhandle = UDT::accept(serv, (sockaddr*)&clientaddr, &addrlen)))
		{
			cout << "accept: " << UDT::getlasterror().getErrorMessage() << endl;
			return ;
		}

		char clienthost[NI_MAXHOST];
		char clientservice[NI_MAXSERV];
		getnameinfo((sockaddr *)&clientaddr, addrlen, clienthost, sizeof(clienthost),
				clientservice, sizeof(clientservice), NI_NUMERICHOST|NI_NUMERICSERV);
		//cout << "new connection: " << clienthost << ":" << clientservice << endl;

		task t;
		t.init(fhandle, g_sPath, (char*)g_szFile, &g_FileIndex);
		tp.push(t);

		if (i++ % 10 == 0)
		{
			printf("use time:%.6f\n", tm.get_time()*1.0/1000000);
		}
	}
}

int main(int argc, char* argv[])
{
	int opt = 0;

	while(( opt = getopt( argc, argv, "f:p:c:")) != -1) {

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

			default /*? h*/:
				showUsage();
				return 0;
		}
	}

	if (g_sPath.empty() || g_sPort.empty())
	{
		showUsage();
		return -1;
	}

	if (!g_sCount.empty() && is_digits(g_sCount) )
	{
		g_iCount = wbl::util::strto<int>(g_sCount);
	}

   // use this function to initialize the UDT library
   UDT::startup();

   addrinfo hints;
   addrinfo* res;

   memset(&hints, 0, sizeof(struct addrinfo));
   hints.ai_flags = AI_PASSIVE;
   hints.ai_family = AF_INET;
   hints.ai_socktype = SOCK_STREAM;


   if (0 != getaddrinfo(NULL, g_sPort.c_str(), &hints, &res))
   {
      cout << "illegal port number or port is busy.\n" << endl;
      return 0;
   }

   UDTSOCKET serv = UDT::socket(res->ai_family, res->ai_socktype, res->ai_protocol);

   int mss = PACK_SIZE;
   UDT::setsockopt(serv, 0, UDT_MSS, &mss, sizeof(int));

   if (UDT::ERROR == UDT::bind(serv, res->ai_addr, res->ai_addrlen))
   {
      cout << "bind: " << UDT::getlasterror().getErrorMessage() << endl;
      return 0;
   }

   freeaddrinfo(res);

   cout << "server is ready at port: " << g_sPort << endl;

   UDT::listen(serv, LISTEN_QUENE_SIZE);

   server(serv);

   UDT::close(serv);
   // use this function to release the UDT library
   UDT::cleanup();

   return 0;
}
