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
#include "FixLoginResponse.h"
#include <string>
#include <v8.h>
#include <node.h>
#include <nan.h>

using namespace v8;

typedef struct fix_event_t {
	std::string eventName;
	Nan::Persistent<v8::Object>* callbacks;
	const FIX::SessionID* sessionId;
	const FIX::Message* message = NULL;
	Nan::Callback* logon = NULL;
	FixLoginResponse* logonResponse;
} fix_event_t;

#endif /* FIXEVENT_H_ */
