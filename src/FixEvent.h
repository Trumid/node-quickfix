/*
 * FixEvent.h
 *
 *  Created on: Jul 14, 2014
 *      Author: kdeol
 */

#ifndef FIXEVENT_H_
#define FIXEVENT_H_
#include "quickfix/Message.h"
#include "quickfix/SessionID.h"
#include <string>

struct FixEvent {
	std::string eventName;
	FIX::Message* message;
	FIX::SessionID* sessionId;
	FixEventHandler* handler;
};

#endif /* FIXEVENT_H_ */
