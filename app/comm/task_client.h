/*
 * taskQueue.h
 *
 *  Created on: 2015年8月30日
 *      Author: gavinlli
 */

#ifndef TASKQUEUE_H_
#define TASKQUEUE_H_

#include <udt.h>

using namespace std;

class task_client
{
public:
	task_client();
	virtual ~task_client();
	virtual void run();
	int get_file();
	void init(int num, string sIp, string sPort, string sPath, struct timeval start, int *iFirst);

private:
	int m_num;
	string m_sIp, m_sPort, m_sPath;
	string m_sFileName;
	struct timeval m_start;
	int *m_iFirst;
};

#endif /* TASKQUEUE_H_ */
