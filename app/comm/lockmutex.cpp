/*
 * lockmutex.cpp
 *
 *  Created on: 2015年8月30日
 *      Author: gavinlli
 */

#include "lockmutex.h"

lock_mutex::lock_mutex() {
	// TODO Auto-generated constructor stub
	pthread_mutex_init(&m_mutex, NULL);
}

lock_mutex::~lock_mutex() {
	// TODO Auto-generated destructor stub

	pthread_mutex_destroy(&m_mutex);
}

void lock_mutex::lock()
{
	pthread_mutex_lock(&m_mutex);
}

void lock_mutex::unlock()
{
	pthread_mutex_unlock(&m_mutex);
}

pthread_mutex_t * lock_mutex::get_mutex()
{
	return &m_mutex;
}
