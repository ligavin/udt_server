/*
 * taskQueue.cpp
 *
 *  Created on: 2015年8月30日
 *      Author: gavinlli
 */

#include "task.h"
#include <iostream>
#include <cstdlib>
#include <netdb.h>
#include <fstream>
#include "wbl_comm.h"
#include <unistd.h>
#include <stdio.h>
#include "tools.h"
#include "timer.h"

using namespace wbl::util;

task::task() {
	// TODO Auto-generated constructor stub

}

void task::init(UDTSOCKET fhandle, string spath, char *szFile, int *iFileIndex)
{
	m_fhandle = fhandle;
	m_spath = spath;
	m_szFile = szFile;
	m_iFileIndex = iFileIndex;
}

task::~task() {
	// TODO Auto-generated destructor stub
}

void task::run()
{
	UDTSOCKET fhandle = m_fhandle;

	// aquiring file name information from client
	int iNum;

	if (UDT::ERROR == UDT::recv(fhandle, (char*)&iNum, sizeof(iNum), 0))
	{
	  cout << "recv error: " << UDT::getlasterror().getErrorMessage() << endl;
	  UDT::close(fhandle);
	  return ;
	}

	string sFolderPath, sFilePath;
	char szFileName[FILE_NAME_SIZE] = "";
	int64_t info[2] = {-1, -1};//size,filename len
	int64_t size = -1;
	info[0] = size;

	bool bExist = GetFileName(m_szFile, m_iFileIndex, iNum, szFileName);

	if (!bExist)
	{
		//cout << "file count:" << *m_iFileIndex << ",Num:" << iNum << "," << "文件不存在" << endl;
	   if (UDT::ERROR == UDT::send(fhandle, (char*)&info, sizeof(int64_t)*2, 0))
	   {
		  cout << "send filesize1: " << UDT::getlasterror().getErrorMessage() << endl;
	   }
	   UDT::close(fhandle);
	   return ;
	}
	//cout << szFileName << endl;

	GetPathByNum(m_spath, szFileName, iNum, sFolderPath, sFilePath);

	fstream ifs(sFilePath.c_str(), ios::in | ios::binary);

	ifs.seekg(0, ios::end);
	size = ifs.tellg();
	ifs.seekg(0, ios::beg);
	int iFileLen = strlen(szFileName);

	/*if (iNum % 4 != 0)
	{
		size = -2;
	}*/

	info[0] = size;
	info[1] = iFileLen;

	// send file size information
	if (UDT::ERROR == UDT::send(fhandle, (char*)&info, sizeof(int64_t)*2, 0))
	{
		cout << "send file size error: " << UDT::getlasterror().getErrorMessage() <<
				",num:" << iNum << "," <<endl;
		UDT::close(fhandle);
		ifs.close();
		return ;
	}

	//cout << "size:" << size << endl;

	if (size == -2)
	{
		//cout << "path:" << sFilePath << ",";
		//cout << "文件跳过" << endl;
		return ;
	}

	if (size <= 0)
	{
		cout << "path:" << sFilePath << ",";
		cout << "文件不存在" << endl;
		return ;
	}

	cout << "path:" << sFilePath << ",文件发送ok" << endl;


	/*
	//发送文件名长度
	if (UDT::ERROR == UDT::send(fhandle, (char*)&iFileLen, sizeof(iFileLen), 0))
	{
		cout << "send filename len error: " << UDT::getlasterror().getErrorMessage() << endl;
		UDT::close(fhandle);
		ifs.close();
		return ;
	}*/

	//发送文件名
	if (UDT::ERROR == UDT::send(fhandle, szFileName, iFileLen, 0))
	{
	  cout << "send filename error: " << UDT::getlasterror().getErrorMessage() << endl;
	  UDT::close(fhandle);
	ifs.close();
	  return ;
	}

	UDT::TRACEINFO trace;
	UDT::perfmon(fhandle, &trace);

	// send the file
	int64_t offset = 0;

	timer tm;
	if (UDT::ERROR == UDT::sendfile(fhandle, ifs, offset, size))
	{
		cout << "sendfile: " << UDT::getlasterror().getErrorMessage() << iNum << szFileName << *m_iFileIndex << endl;
		UDT::close(fhandle);
		ifs.close();
		return ;
	}

	UDT::perfmon(fhandle, &trace);
	//cout << "speed = " << trace.mbpsSendRate << "Mbits/sec," ;
	//printf("  use time:%.3f\n", tm.get_time()*1.0/1000000);

	UDT::close(fhandle);

	ifs.close();

	return ;
}
