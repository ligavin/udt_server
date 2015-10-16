/*
 * lockmutex.h
 *
 *  Created on: 2015年8月30日
 *      Author: gavinlli
 */

#ifndef LOCKMUTEX_H_
#define LOCKMUTEX_H_

#include <pthread.h>


class lock_mutex
{
public:
	lock_mutex();
	virtual ~lock_mutex();
	void lock();
	void unlock();
	pthread_mutex_t * get_mutex();

private:
	pthread_mutex_t m_mutex;
};


#endif /* LOCKMUTEX_H_ */
