/*
 * bin_2_order.cpp
 *
 *  Created on: 2015年10月11日
 *      Author: gavinlli
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sstream>

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
std::vector<Order> vecOrder;

void ReadMem(void *src, int &index, void *dst, int len)
{
	memcpy(dst, src + (size_t)index, (size_t)len);
	index += len;
}

template<typename T> T strto(const std::string& s)
{
	std::istringstream is(s);
	T t;
	is >> t;
	return t;
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
	iHours = (strto<int>(sTime.substr(0,2)));
	iMin = strto<int>(sTime.substr(2,2));
	iSec = strto<int>(sTime.substr(4,2));

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

char *buf[10000000];

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
				cout << "orderid1:" << iOrderId << ",dPosx:" << order.iOrderId << ",poxy:" << dPosy << endl;
			}
			else
				cout << "orderid2:" << iOrderId << ",dPosx:" << dPosx << ",poxy:" << dPosy << endl;

		}
		else
		{
			Order order = {iOrderId, dPosx, dPosy, 0, iWorkLen, iPrice};
			vecOrder.push_back(order);
			cout << "orderid3:" << iOrderId << ",dPosx:" << order.iOrderId << ",poxy:" << dPosy << endl;
		}

		cout << "111:" << fin.tellg() <<  ",iLeft:" << iLenLeft << ",recordlen:" << iRecordLen << endl;
		//fin.seekg(iLenLeft, ios::cur);
		fin.read((char *)buf, iLenLeft);
		cout << "222:" << fin.tellg() << endl;
	}

	fin.close();
}


int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		cout << "no filename" << endl;
		return 0;
	}/*
	char *file = new char[MAX_FILE_SIZE];
	int iFileLen = 0;

	FILE *inFile;
	inFile = fopen(argv[1], "rb");

	if (!inFile)
	{
		cout << "open file failed" << endl;
		return -1;
	}


	int nRead = 0;
	while((nRead = fread(file + iFileLen,  sizeof(unsigned char), BUFF_SIZE, inFile)) > 0)
	{
		iFileLen += nRead;
		nRead = 0;
	}

	vector<Order> vecOrder;


	struct timeval start;
	gettimeofday( &start, NULL);

	ReadData(file, argv[1], iFileLen, vecOrder);
	delete[] file;


	struct timeval end;
	gettimeofday( &end, NULL);
	int timeuse = (end.tv_sec - start.tv_sec ) * 1000000 + (end.tv_usec - start.tv_usec);
*/
	vector<Order> vecOrder;
	ReadData1(argv[1], vecOrder);
	//printf("time used:%.6f\n", timeuse*1.0/1000000);

	for(std::vector<Order>::iterator it= vecOrder.begin(); it!=vecOrder.end(); it++)
	{

		cout<<it->iOrderId<<"\t"     <<it->dPosx<<"\t"     <<it->dPosy<<"\ttime:"
			<<it->iStartTime<< "\thours:" << it->iStartTime*1.0/60 << "\t"
			<<it->iWorkLen<<"\t"  <<it->iPrice<<endl;
	}
	cout << "vec size:" << vecOrder.size() << endl;
	return 0;
}
