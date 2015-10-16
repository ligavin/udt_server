/*
 * lock.h
 *
 *  Created on: 2015年9月3日
 *      Author: gavinlli
 */

#ifndef LOCK_H_
#define LOCK_H_

template<class MUTEX>
class lock
{
public:
	lock(MUTEX &mutex);
	virtual ~lock();
private:
	MUTEX &m_mutex;
};



template<class MUTEX>
lock<MUTEX>::lock(MUTEX &mutex):m_mutex(mutex)
{
	m_mutex.lock();
}

template<class MUTEX>
lock<MUTEX>::~lock()
{
	m_mutex.unlock();
}



#endif /* LOCK_H_ */
