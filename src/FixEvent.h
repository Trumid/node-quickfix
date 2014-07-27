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
#include <v8.h>
#include <node.h>
#include <nan.h>

typedef struct {
	std::string eventName;
	const FIX::SessionID* sessionId;
	const FIX::Message* message;
	v8::Persistent<v8::Object>* callbacks;
} fix_event_t;

#endif /* FIXEVENT_H_ */
