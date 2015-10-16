/*
 * parsefile.cpp
 *
 *  Created on: 2015Äê10ÔÂ11ÈÕ
 *      Author: gavinlli
 */




#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUFF_SIZE 10000

using namespace std;

struct Order
{
	int iOrderId;
	double dPosx;
	double dPosy;
	int iStartTime;
	int iWorkLen;
	int iPrice;
};
std::vector<Order> vecOrder;

void ReadData()
{
	ifstream fin("test.dat", ios::binary);
	int iRecordLen;
	char buff[BUFF_SIZE];

	int iOrderId, iPrice, iWorkLen, iLimit, iStartTime;
	double dPosx, dPosy;
	char chWorkLen, chLimit;

	while(fin.read((char*)(&iRecordLen), 4))
	{
		memset(buff, 0, BUFF_SIZE);
		fin.read(buff, 1);
		fin.read((char *)(&iOrderId), 4);
		fin.read((char *)(&dPosx), 8);
		fin.read((char *)(&dPosy), 8);
		fin.read((char *)(&iPrice), 4);
		fin.read((char *)(&chWorkLen), 1);
		fin.read((char *)(&chLimit), 1);

		iWorkLen = chWorkLen;
		iLimit   = chLimit;

		int iLenLeft = iRecordLen - 31;
		if(iLimit == 1)
		{
			iLenLeft -= 4;
			fin.read((char *)(&iStartTime), 4);

			Order order = {iOrderId, dPosx, dPosy, iStartTime, iWorkLen, iPrice};
			vecOrder.push_back(order);
		}
		else
		{
			Order order = {iOrderId, dPosx, dPosy, 0, iWorkLen, iPrice};
			vecOrder.push_back(order);
		}

		while(iLenLeft > BUFF_SIZE)
		{
			fin.read(buff, BUFF_SIZE);
			iLenLeft -= BUFF_SIZE;
		}
		fin.read(buff, iLenLeft);
	}
}

int main()
{
	ReadData();

	char filename[] = "result.txt";
	ofstream fout(filename);

	for(std::vector<Order>::iterator it= vecOrder.begin(); it!=vecOrder.end(); it++)
	{
		fout<<it->iOrderId<<"\t"     <<it->dPosx<<"\t"     <<it->dPosy<<"\t" \
			<<it->iStartTime<<"\t"   <<it->iWorkLen<<"\t"  <<it->iPrice<<endl;
	}
	return 0;
}
