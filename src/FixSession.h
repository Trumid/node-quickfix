/*
 * FixSession.h
 *
 *  Created on: Sep 16, 2014
 *      Author: kdeol
 */

#ifndef FIXSESSION_H_
#define FIXSESSION_H_

#include <v8.h>
#include <node.h>
#include <nan.h>

#include "quickfix/Session.h"

using namespace v8;
using namespace node;

class FixSession : public Nan::ObjectWrap {
public:
	FixSession();
	static void Initialize();
	static NAN_METHOD(New);
	void setSession(FIX::Session* session);
	static Handle<Object> wrapFixSession(FIX::Session *session);

private:
	virtual ~FixSession();
	FIX::Session* mSession;

	static NAN_METHOD(disconnect);
	static NAN_METHOD(getSessionID);
	static NAN_METHOD(isEnabled);
	static NAN_METHOD(isLoggedOn);
	static NAN_METHOD(logon);
	static NAN_METHOD(logout);
	static NAN_METHOD(refresh);
	static NAN_METHOD(reset);

	static NAN_GETTER(getSenderSeqNum);
	static NAN_SETTER(setSenderSeqNum);

	static NAN_GETTER(getTargetSeqNum);
	static NAN_SETTER(setTargetSeqNum);
	
	
};

#endif /* FIXSESSION_H_ */
