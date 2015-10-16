/*
 * taskQueue.h
 *
 *  Created on: 2015年8月30日
 *      Author: gavinlli
 */

#ifndef TASKQUEUE_H_
#define TASKQUEUE_H_

#include <udt.h>
#include "info.h"

using namespace std;

class task
{
public:
	task();
	virtual ~task();
	virtual void run();
	void init(UDTSOCKET fhandle, string spath, char *szFile, int *iFileIndex);

private:
	int m_num;
	UDTSOCKET m_fhandle;
	string m_spath;
	char *m_szFile;
	int *m_iFileIndex;

};

#endif /* TASKQUEUE_H_ */
