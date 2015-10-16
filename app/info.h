/*
 * info.h
 *
 *  Created on: 2015Äê10ÔÂ9ÈÕ
 *      Author: gavinlli
 */

#ifndef INFO_H_
#define INFO_H_

#include <stdio.h>
#include <string>

using namespace std;


#define FILE_BIT (1<<15)

#define MAKE_NUM(x,y) ((x<<16)+(y))
#define GET_HIGH(v) (v>>16)
#define GET_LOW(v) (v&(1<<15))
#define LISTEN_QUENE_SIZE 60
#define PACK_SIZE 1052
#define THREAD_POOL_QUEUE_SIZE 60
#define CLIENT_THREAD_POOL_QUEUE_SIZE 60

#define GET_H_NUM(v) (v/100)
#define GET_L_NUM(v) (v%100)

#define FILE_NUM 10024
#define FILE_NAME_SIZE 105


#endif /* INFO_H_ */
