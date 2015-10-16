/*
 * timer.cpp
 *
 *  Created on: 2015年9月3日
 *      Author: gavinlli
 */

#include "timer.h"
#include <stdio.h>


timer::timer() {
	// TODO Auto-generated constructor stub
	start();
}

timer::~timer() {
	// TODO Auto-generated destructor stub
}

void timer::start()
{
	gettimeofday( &m_start, NULL);
}
int timer::get_time()
{
	struct timeval end;
	gettimeofday( &end, NULL);
	int timeuse = (end.tv_sec - m_start.tv_sec ) * 1000000 + (end.tv_usec - m_start.tv_usec);
	return timeuse;
}
