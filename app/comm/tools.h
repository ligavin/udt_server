/*
 * toools.h
 *
 *  Created on: 2015年8月29日
 *      Author: gavinlli
 */

#include <sstream>
#include<string>

using namespace std;

template<typename T> std::string tostr(const T& t)
{
	std::ostringstream s;
	s << t;
	return s.str();
}

template<typename T> T strto(const std::string& s)
{
	std::istringstream is(s);
	T t;
	is >> t;
	return t;
}

void Daemon();

void GetPathByNum(const string &spath, const string &sFileName, const int &iNum,
		string &sFolderPath, string &sFilePath);
bool AddFile(char *szFile, int &iFileIndex, char *FileName);
bool GetFileName(char *m_szFile, int *iFileIndex, const int &iNum, char *szFileName);
