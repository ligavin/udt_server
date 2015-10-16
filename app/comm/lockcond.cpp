/*
 * lock_cond.cpp
 *
 *  Created on: 2015年9月3日
 *      Author: gavinlli
 */

#include "lockcond.h"
#include <sys/time.h>


lock_cond::lock_cond(){
	// TODO Auto-generated constructor stub
	pthread_cond_init(&m_cond, NULL);
}

lock_cond::~lock_cond() {
	// TODO Auto-generated destructor stub
	pthread_cond_destroy(&m_cond);
}

void lock_cond::wait(pthread_mutex_t *mutex)
{
	 pthread_cond_wait(&m_cond,mutex);
}

void lock_cond::wait_with_time(pthread_mutex_t *mutex, unsigned sec,unsigned us)
{
	struct timeval now;
	struct timespec time_out;
	gettimeofday(&now, NULL);
	time_out.tv_sec = now.tv_sec + sec;
	time_out.tv_nsec = now.tv_usec * 1000 + us;//1s=10^9nsec
	pthread_cond_timedwait(&m_cond, mutex, &time_out);
}

void lock_cond::post_msg()
{
	pthread_cond_signal(&m_cond);
}
