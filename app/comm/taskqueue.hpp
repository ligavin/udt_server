/*
 * bufferqueue.h
 *
 *  Created on: 2015年9月4日
 *      Author: gavinlli
 */

#ifndef BUFFERQUEUE_H_
#define BUFFERQUEUE_H_

#include "lock.hpp"
#include "lockmutex.h"
#include "lockcond.h"
#include <stdio.h>
#include <queue>

template <typename Ttask, typename Sequence = std::deque<Ttask> >

class task_queue
{
private:
	typedef std::queue<Ttask, Sequence> Queue;
	Queue reqQue;
	lock_mutex m_locker;
	lock_cond m_cond;
	unsigned m_maxQueSize;


public:
	task_queue(unsigned maxQueSize): m_maxQueSize(maxQueSize)
	{
		// TODO Auto-generated constructor stub

	}

	~task_queue()
	{
		// TODO Auto-generated destructor stub
	}

	bool push(const Ttask &t)
	{
		lock<lock_mutex> locker(m_locker);

		if (reqQue.size() >= m_maxQueSize)
		{
			return false;
		}

		reqQue.push(t);
		m_cond.post_msg();
		return true;
	}

	bool pop(Ttask &t)
	{
		lock<lock_mutex> locker(m_locker);

		if (reqQue.empty())
		{
			m_cond.wait_with_time(m_locker.get_mutex(),0,10*1000);

			if (reqQue.empty())
			{
				return false;
			}
		}

		t = reqQue.front();
		reqQue.pop();

		return true;
	}

	unsigned get_queue_size()
	{
		lock<lock_mutex> locker(m_locker);
		return reqQue.size();
	}
};

#endif /* BUFFERQUEUE_H_ */
