/*
 * tools.cpp
 *
 *  Created on: 2015年8月29日
 *      Author: gavinlli
 */

#include<unistd.h>
#include<stdlib.h>
#include<signal.h>
#include<sys/types.h>
#include<sys/stat.h>
#include "tools.h"
#include "info.h"
#include <iostream>
#include <cstring>

using namespace std;

void Daemon()
{
	pid_t pid;

	if ((pid = fork()) != 0) { exit(0); }

	setsid();
	signal(SIGINT, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);
	signal(SIGTERM, SIG_IGN);

	// ignore_pipe();
	struct sigaction sig;
	sig.sa_handler = SIG_IGN;
	sig.sa_flags = 0;
	sigemptyset(&sig.sa_mask);
	sigaction(SIGPIPE, &sig, NULL);

	if ((pid = fork()) != 0) 	{ exit(0);	}

	//chdir("/");

	umask(0);

	for (int i = 3; i < 64; i++)
		close(i);
}

void GetPathByNum(const string &spath, const string &sFileName, const int &iNum,
		string &sFolderPath, string &sFilePath)
{
	int iFolder;

	iFolder = GET_H_NUM(iNum);

	char szFolder[5];

	snprintf(szFolder, sizeof(szFolder), "%02d", iFolder);

	sFolderPath = spath + (string)"/" + (string)szFolder;
	sFilePath = spath + (string)"/" + (string)szFolder + "/" + sFileName;
}

char *GetAddr(char *p, int rows)
{
	return p + rows * FILE_NAME_SIZE;
}

bool AddFile(char *szFile, int &iFileIndex, char *FileName)
{
	if (iFileIndex >= FILE_NUM - 1)
		return false;

	char szNum[6] = "";
	snprintf(szNum, sizeof(szNum), "%05d", iFileIndex);

	if (strlen(FileName) < 5 || strncmp(FileName, szNum, 5))
	{
		cout << "file name not ok:" << FileName << ",it should be:" << szNum << "-***.dat"<<endl;
		return false;
	}

	snprintf(GetAddr(szFile, iFileIndex), FILE_NAME_SIZE, "%s", FileName);

	//cout << GetAddr(szFile, iFileIndex) << endl;

	iFileIndex ++ ;
	return true;
}

bool GetFileName(char *szFile, int *iFileIndex, const int &iNum, char *szFileName)
{
	if (iNum >= *iFileIndex)
	{
		return false;
	}

	snprintf(szFileName, FILE_NAME_SIZE, "%s", GetAddr(szFile, iNum));
	return true;
}
