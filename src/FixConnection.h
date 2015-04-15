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

#include <unordered_set>
#include "quickfix/Application.h"
#include "quickfix/SessionID.h"
#include "quickfix/Message.h"
#include "quickfix/SocketInitiator.h"
#include "quickfix/MessageStore.h"
#include "quickfix/FileStore.h"
#include "quickfix/FileLog.h"
#include "quickfix/config.h"
#ifdef HAVE_POSTGRESQL
#include "quickfix/PostgreSQLStore.h"
#include "quickfix/PostgreSQLLog.h"
#endif
#ifdef HAVE_MYSQL
#include "quickfix/MySQLStore.h"
#include "quickfix/MySQLLog.h"
#endif
#ifdef HAVE_ODBC
#include "quickfix/OdbcStore.h"
#include "quickfix/OdbcLog.h"
#endif

#include "FixApplication.h"

using namespace v8;
using namespace node;

class FixConnection : public node::ObjectWrap {
public:
	FixConnection(FIX::SessionSettings settings, std::string storeFactory);
	FixConnection(FixApplication* application, FIX::SessionSettings settings, std::string storeFactory);

private:

protected:
	~FixConnection();
	void setup(FIX::SessionSettings settings, std::string storeFactory);

	FixApplication* mFixApplication;
	FIX::MessageStoreFactory* mStoreFactory;
	FIX::LogFactory* mLogFactory;

	FIX::SessionSettings mSettings;
	v8::Persistent<v8::Object> mCallbacks;
	std::unordered_set<std::string> mCallbackRegistry;
};

#endif /* FIXCONNECTION_H_ */
