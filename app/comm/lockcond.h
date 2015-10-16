/*
 * lockcond.h
 *
 *  Created on: 2015年9月3日
 *      Author: gavinlli
 */

#ifndef LOCKCOND_H_
#define LOCKCOND_H_

#include <pthread.h>

class lock_cond {
public:
	lock_cond();
	virtual ~lock_cond();
	void wait(pthread_mutex_t *mutex);
	void wait_with_time(pthread_mutex_t *mutex, unsigned sec = 0,unsigned us = 1000000);
	void post_msg();

private:
	pthread_cond_t m_cond;
};


#endif /* LOCKCOND_H_ */
