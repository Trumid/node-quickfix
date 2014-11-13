/*
 * FixEventQueue.h
 *
 *  Created on: Nov 12, 2014
 *      Author: kdeol
 */

#ifndef FIXEVENTQUEUE_H_
#define FIXEVENTQUEUE_H_
#include "tbb/concurrent_queue.h"
#include "FixEvent.h"

typedef struct fix_event_queue_t {
	tbb::concurrent_queue<fix_event_t*>* queue;
} fix_event_queue_t;


#endif /* FIXEVENTQUEUE_H_ */
