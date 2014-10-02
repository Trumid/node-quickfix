/*
 * FixConnection.h
 *
 *  Created on: Sep 9, 2014
 *      Author: kdeol
 */

#ifndef FIXCONNECTION_H_
#define FIXCONNECTION_H_

#include <v8.h>
#include <node.h>
#include <nan.h>

#include "quickfix/Application.h"
#include "quickfix/SessionID.h"
#include "quickfix/Message.h"
#include "quickfix/SocketInitiator.h"
#include "FixApplication.h"
#include "FixLogonEvent.h"

#include "quickfix/FileStore.h"
#include "quickfix/FileLog.h"

using namespace v8;
using namespace node;

class FixConnection : public node::ObjectWrap {
public:
	FixConnection(const char* propertiesFile);

private:

protected:
	~FixConnection();
	uv_async_t mAsyncFIXEvent;
	uv_async_t mAsyncLogonEvent;

	FixApplication* mFixApplication;
	FIX::FileStoreFactory* mStoreFactory;
	FIX::FileLogFactory* mLogFactory;

	FIX::SessionSettings mSettings;
	Persistent<Object> mCallbacks;
	FixLoginProvider* mFixLoginProvider;
	static void handleLogonEvent(uv_async_t *handle, int status);
};

#endif /* FIXCONNECTION_H_ */
