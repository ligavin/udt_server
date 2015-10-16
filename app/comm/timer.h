/*
 * timer.h
 *
 *  Created on: 2015年9月3日
 *      Author: gavin
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <sys/time.h>

class timer {
public:
	timer();
	void start();
	int get_time();
	virtual ~timer();

private:
	struct timeval m_start;
};

#endif /* TIMER_H_ */
