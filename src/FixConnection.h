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

#include "quickfix/MessageStore.h"
#include "quickfix/FileStore.h"
#ifdef HAVE_POSTGRESQL
#include "quickfix/config.h"
#include "quickfix/PostgreSQLStore.h"
#include "quickfix/PostgreSQLLog.h"
#endif
#ifdef HAVE_MYSQL
#include "quickfix/config.h"
#include "quickfix/MySQLStore.h"
#include "quickfix/MySQLLog.h"
#endif
#ifdef HAVE_ODBC
#include "quickfix/config.h"
#include "quickfix/OdbcStore.h"
#include "quickfix/OdbcLog.h"
#endif
#include "quickfix/FileLog.h"

using namespace v8;
using namespace node;

class FixConnection : public node::ObjectWrap {
public:
	FixConnection(const char* propertiesFile, std::string storeFactory);

private:

protected:
	~FixConnection();
	uv_async_t mAsyncFIXEvent;
	uv_async_t mAsyncLogonEvent;

	FixApplication* mFixApplication;
	FIX::MessageStoreFactory* mStoreFactory;
	FIX::LogFactory* mLogFactory;

	FIX::SessionSettings mSettings;
	Persistent<Object> mCallbacks;
	FixLoginProvider* mFixLoginProvider;
	static void handleLogonEvent(uv_async_t *handle, int status);
};

#endif /* FIXCONNECTION_H_ */
