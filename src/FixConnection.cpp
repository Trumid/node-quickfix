/*
 * FixConnection.cpp
 *
 *  Created on: Sep 9, 2014
 *      Author: kdeol
 */

#include "FixConnection.h"
#include "FixMessageUtil.h"
#include <v8.h>
#include <node.h>
#include <nan.h>
#include "FixLogonEvent.h"
#include "quickfix/SessionID.h"
#include "quickfix/Message.h"

using namespace v8;
using namespace node;

FixConnection::FixConnection(const char* propertiesFile): ObjectWrap()  {
	mSettings = FIX::SessionSettings(propertiesFile);
	mFixApplication = new FixApplication(
			&mAsyncFIXEvent, &mAsyncLogonEvent, &mCallbacks);
	mStoreFactory = new FIX::FileStoreFactory(mSettings);
	mLogFactory = new FIX::FileLogFactory(mSettings);
}

FixConnection::~FixConnection() {
	uv_close((uv_handle_t*) &mAsyncFIXEvent, NULL);
	uv_close((uv_handle_t*) &mAsyncLogonEvent, NULL);
}

void FixConnection::handleLogonEvent(uv_async_t *handle, int status) {
	NanScope();

	fix_logon_t* logonEvent = (fix_logon_t*)handle->data;

	Local<Object> msg = NanNew<Object>();

	FixMessageUtil::fix2Js(msg, logonEvent->message);

	Local<Value> argv[] = {
			msg,
			NanNew<String>(logonEvent->sessionId->toString().c_str())
	};

	logonEvent->logon->Call(2, argv);

};
