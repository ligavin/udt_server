/*
 * taskQueue.cpp
 *
 *  Created on: 2015年8月30日
 *      Author: gavinlli
 */

#include "task_client.h"
#include <iostream>
#include <cstdlib>
#include <netdb.h>
#include <fstream>
#include "wbl_comm.h"
#include <unistd.h>
#include <stdio.h>
#include "info.h"
#include <fcntl.h>
#include "tools.h"
#include "timer.h"

using namespace wbl::util;

task_client::task_client() {
	// TODO Auto-generated constructor stub

}

void task_client::init(int num, string sIp, string sPort, string sPath, struct timeval start, int *iFirst)
{
	m_num = num;
	m_sIp = sIp;
	m_sPort = sPort;
	m_sPath = sPath;
	m_start = start;
	m_iFirst = iFirst;
}

task_client::~task_client() {
	// TODO Auto-generated destructor stub
}

int task_client::get_file()
{
	// use this function to initialize the UDT library
	//UDT::startup();

	int time1 = 0,time2 = 0,time3 = 0, time4 = 0;
	timer tm;

	struct addrinfo hints, *peer;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	UDTSOCKET fhandle = UDT::socket(hints.ai_family, hints.ai_socktype, hints.ai_protocol);

	int iTimeOut = 1000;

	UDT::setsockopt(fhandle,0, UDT_RCVTIMEO,&iTimeOut, 0);
	UDT::setsockopt(fhandle,0, UDT_SNDTIMEO,&iTimeOut, 0);

	if (0 != getaddrinfo(m_sIp.c_str(), m_sPort.c_str(), &hints, &peer))
	{
	  cout << "incorrect server/peer address. " << m_sIp << ":" << m_sPort << endl;
	  UDT::close(fhandle);
		// use this function to release the UDT library
		//UDT::cleanup();
	  return -1;
	}

	if (UDT::ERROR == UDT::connect(fhandle, peer->ai_addr, peer->ai_addrlen))
	{
	  cout << "connect: " << UDT::getlasterror().getErrorMessage() << endl;
	  UDT::close(fhandle);
	  //UDT::cleanup();
	  return -1;
	}
	time1 = tm.get_time();
	//cout << "connect ok" << endl;

	freeaddrinfo(peer);


	// send name information of the requested file

	int64_t size = 0;
	int64_t info[2] = {0,0};

	size = 0;
	//cout << "num:" << m_num << endl;
	if (UDT::ERROR == UDT::send(fhandle, (char*)&m_num, sizeof(int), 0))
	{
	  cout << "send: " << UDT::getlasterror().getErrorMessage() << endl;
	  UDT::close(fhandle);
	 // UDT::cleanup();
	  return -1;
	}

	size = 0;


	//获取文件大小
	if (UDT::ERROR == UDT::recv(fhandle, (char*)&info, sizeof(int64_t)*2, 0))
	{
		cout << "send: " << UDT::getlasterror().getErrorMessage() << endl;
		 UDT::close(fhandle);
		return -1;
	}
	time2 = tm.get_time();

	size = info[0];

	if (size == -2)
	{
		return -2;
	}

	if (size <= 0)
	{
		//cout << "文件不存在,sleep,num:" << m_num << endl;
		 //UDT::close(fhandle);
		return -3;
	}

	//先拿到文件名长度
	int iFileNameLen = info[1];
	char szFileName[FILE_NAME_SIZE];
	/*if (UDT::ERROR == UDT::recv(fhandle, (char*)&iFileNameLen, sizeof(iFileNameLen), 0))
	{
	  cout << "recv filename len error: " << UDT::getlasterror().getErrorMessage() << endl;
	  UDT::close(fhandle);
	  return -1;
	}*/
	//文件名传输
	if (UDT::ERROR == UDT::recv(fhandle, szFileName, iFileNameLen, 0))
	{
	  cout << "recv: " << UDT::getlasterror().getErrorMessage() << endl;
	  UDT::close(fhandle);
	  return -1;
	}
	szFileName[iFileNameLen] = '\0';


	string sFolderPath, sFilePath, sFileName = szFileName;

	GetPathByNum(m_sPath, sFileName, m_num, sFolderPath, sFilePath);

	if (access(sFolderPath.c_str(), F_OK) != 0)
	{
		cout << "create dir" << sFolderPath << endl;
		int iRet = mkdir(sFolderPath.c_str(), 0777);
		cout << "mkdir ok:" << iRet << endl;
	}

	// receive the file
	fstream ofs(sFilePath.c_str(), ios::out | ios::binary | ios::trunc);
	int64_t recvsize;
	int64_t offset = 0;

	if (!ofs)
	{
		cout << "open file failed:" << sFilePath << endl;
		UDT::close(fhandle);
		return -1;
	}

	time3 = tm.get_time();
	UDT::TRACEINFO trace;
	UDT::perfmon(fhandle, &trace);

	if (UDT::ERROR == (recvsize = UDT::recvfile(fhandle, ofs, offset, size)))
	{
	  cout << "recvfile: " << UDT::getlasterror().getErrorMessage() << ",filename:" << szFileName << endl;
	  UDT::close(fhandle);
	  ofs.close();
	  return -1;
	}
	m_sFileName = sFilePath;
	UDT::perfmon(fhandle, &trace);

	time4 = tm.get_time();

	if (m_num % 10 == 0)
	{
		cout << "path:" << m_num ;
		printf("  use time:%.3f,size:%.3f", tm.get_time()*1.0/1000000, size*1.0/1024/1024);
		printf(",speed:%.3fM/s,  time1:%.3f",  trace.mbpsRecvRate, time1*1.0/1000000);
		printf("  time2:%.3f", (time2-time1)*1.0/1000000);
		printf("  time3:%.4f", (time3-time2)*1.0/1000000);
		printf("  time4:%.3f\n", (time4-time3)*1.0/1000000);
	}

	UDT::close(fhandle);

	ofs.close();


	return 0;
}


#define BUFF_SIZE 1024
#define MAX_FILE_SIZE 1024*1024*15

using namespace std;

struct Order
{
	int iOrderId;
	double dPosx;
	double dPosy;
	int iStartTime;//0代表没有时间要求
	int iWorkLen;
	int iPrice;
};
//std::vector<Order> vecOrder;

void ReadMem(void *src, int &index, void *dst, int len)
{
	memcpy(dst, src + (size_t)index, (size_t)len);
	index += len;
}

int GetFileTime(const string &file_name)
{
	size_t startpos = file_name.find_first_of('-');
	size_t endpos = file_name.find_first_of('.');

	if (startpos == string::npos || endpos == string::npos)
	{
		cout << "file name not valid" << endl;
		return 0;
	}

	string sTime = file_name.substr(startpos+1, endpos-startpos-1);

	int iHours = 0, iMin = 0, iSec;
	iHours = (wbl::util::strto<int>(sTime.substr(0,2)));
	iMin = wbl::util::strto<int>(sTime.substr(2,2));
	iSec = wbl::util::strto<int>(sTime.substr(4,2));

	return (iHours * 60 + iMin) * 60 + iSec;
}

void ReadData(void *szFile, const string &file_name, const int &size, vector<Order> &vecOrder)
{
	int iFileTime = 0;

	iFileTime = GetFileTime(file_name);

	vecOrder.clear();

	int iOrderId, iPrice, iWorkLen, iLimit, iStartTime;
	double dPosx, dPosy;
	char chWorkLen, chLimit;
	int iRecordLen = 0;

	int index = 0;

	while(index < size)
	{
		ReadMem(szFile, index, (void *)&iRecordLen, 4);
		index++;
		ReadMem(szFile, index, (void *)&iOrderId, 4);

		ReadMem(szFile, index, (void *)&dPosx, 8);
		ReadMem(szFile, index, (void *)&dPosy, 8);
		ReadMem(szFile, index, (void *)&iPrice, 4);
		ReadMem(szFile, index, (void *)&chWorkLen, 1);
		ReadMem(szFile, index, (void *)&chLimit, 1);
		iWorkLen = chWorkLen;
		iLimit   = chLimit;

		int iLenLeft = iRecordLen - 31;
		if(iLimit == 1)
		{
			iLenLeft -= 4;
			ReadMem(szFile, index, (void *)&iStartTime, 4);


			if (iStartTime * 60 > iFileTime)
			{
				Order order = {iOrderId, dPosx, dPosy, iStartTime, iWorkLen, iPrice};
				vecOrder.push_back(order);
			}
			else
			{
				//cout << "startTime:" << iStartTime << ",filetime:" << iFileTime / 60 << endl;
			}
		}
		else
		{
			Order order = {iOrderId, dPosx, dPosy, 0, iWorkLen, iPrice};
			vecOrder.push_back(order);
		}
		index += iLenLeft;
	}
}

void ReadData1(const string& file, vector<Order> &vecOrder)
{
	vecOrder.clear();
	double ret;
	long len = 0;

	ifstream fin(file.c_str(), ios::binary);
	if(!fin)
	{
		cout<<file<<"-----error: file not exist-----"<<endl;
		return ;
	}

	int iFileTime = 0;

	iFileTime = GetFileTime(file);

	int iRecordLen;
	char buff[BUFF_SIZE];

	int iOrderId, iPrice, iWorkLen, iLimit, iStartTime;
	double dPosx, dPosy;
	char chWorkLen, chLimit;

	char tmp[35];

	while(fin.read((char*)(&iRecordLen), 4))
	{
		len += iRecordLen;

		fin.read(tmp, 31);
		int index = 1;
		ReadMem((void*)tmp, index, (void*)&iOrderId, 4);
		ReadMem((void*)tmp, index, (void*)&dPosx, 8);
		ReadMem((void*)tmp, index, (void*)&dPosy, 8);
		ReadMem((void*)tmp, index, (void*)&iPrice, 4);
		ReadMem((void*)tmp, index, (void*)&chWorkLen, 1);
		ReadMem((void*)tmp, index, (void*)&chLimit, 1);

		iWorkLen = chWorkLen;
		iLimit   = chLimit;

		int iLenLeft = iRecordLen - 31;

		cout << "orderid:" << iOrderId << ",dPosx:" << dPosx << ",poxy:" << dPosy << endl;
		if(iLimit == 1)
		{
			iLenLeft -= 4;
			fin.read((char *)(&iStartTime), 4);

			if (iStartTime * 60 - 1 > iFileTime)
			{
				Order order = {iOrderId, dPosx, dPosy, iStartTime, iWorkLen, iPrice};
				vecOrder.push_back(order);
				cout << "orderid:" << iOrderId << ",dPosx:" << order.iOrderId << ",poxy:" << dPosy << endl;
			}

		}
		else
		{
			Order order = {iOrderId, dPosx, dPosy, 0, iWorkLen, iPrice};
			vecOrder.push_back(order);
		}

		fin.seekg(iLenLeft, ios::cur);
	}

	fin.close();
}


void saveFile(const string &sFileName, int *iFirst)
{
	char *file = new char[MAX_FILE_SIZE];
	int iFileLen = 0;

	FILE *inFile;
	inFile = fopen(sFileName.c_str(), "rb");

	if (!inFile)
	{
		cout << "open file failed" << endl;
		return ;
	}


	int nRead = 0;
	while((nRead = fread(file + iFileLen,  sizeof(unsigned char), BUFF_SIZE, inFile)) > 0)
	{
		iFileLen += nRead;
		nRead = 0;
	}

	fclose(inFile);

	vector<Order> vecOrder;


	struct timeval start;
	gettimeofday( &start, NULL);

	ReadData(file, sFileName.c_str(), iFileLen, vecOrder);
	delete[] file;


	struct timeval end;
	gettimeofday( &end, NULL);
	int timeuse = (end.tv_sec - start.tv_sec ) * 1000000 + (end.tv_usec - start.tv_usec);
/*

	vector<Order> vecOrder;
	ReadData1(sFileName, vecOrder);
*/
	string result_file = "../result/result_file";

	int result_fd = -1;

	if (0 == *iFirst)
	{
		result_fd = open(result_file.c_str(), O_WRONLY|O_CREAT|O_TRUNC|O_APPEND, S_IRWXU);

		if ( result_fd < 0 )
		{
			printf("open %s fail: %d, %s\n", result_file.c_str(), errno, strerror(errno));
			return ;
		}

		*iFirst = 1;

		unsigned int uiNow = time(NULL);

		char buf[15];
		int write_len = snprintf(buf, sizeof(buf), "%u\n", uiNow);

		write(result_fd, buf, write_len);

	}
	else
	{
		result_fd = open(result_file.c_str(), O_WRONLY|O_APPEND, S_IRWXU);

		if ( result_fd < 0 )
		{
			printf("open %s fail: %d, %s\n", result_file.c_str(), errno, strerror(errno));
			return ;
		}
	}

	for ( unsigned int i = 0; i < vecOrder.size(); i++)
	{
		int limit = 1;
		if ( vecOrder[i].iStartTime == 0 )
			limit = 0;
		char buf[1024];
		int write_len = snprintf(buf, sizeof(buf), "%d,%f,%f,%d,%d,%d,%d\n",
					vecOrder[i].iOrderId,
					vecOrder[i].dPosx,
					vecOrder[i].dPosy,
					vecOrder[i].iWorkLen,
					limit,
					vecOrder[i].iStartTime,
					vecOrder[i].iPrice
					);

		write(result_fd, buf, write_len);
	}
	close(result_fd);
}

void task_client::run()
{
	int iRet = -1, i = 0;
	while(true)
	{
		/*struct timeval end;
		gettimeofday( &end, NULL);
		int timeUse = end.tv_sec - m_start.tv_sec;
		if (m_num <= (10000 * (timeUse * 1.0/300) - 300 ))
		{
			cout << "break, m_num:" << m_num << ",timeuse:" << timeUse <<
					",real num:" << (10000 * (timeUse * 1.0/300) - 800 ) << endl;
			break;
		}*/


		iRet = get_file();

		if (iRet == 0)
		{
			break;
		}

		/*if (iRet == -1)
		{
			usleep(500*1000);
			break;
		}*/

		usleep(200*1000);
		//cout << "num:" << m_num << ",ret:" << iRet << endl;
	}


	if (iRet < 0)
		return ;

	saveFile(m_sFileName, m_iFirst);
}
