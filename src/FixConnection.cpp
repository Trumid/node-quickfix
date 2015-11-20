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
#include "quickfix/Dictionary.h"
#include "quickfix/SessionID.h"
#include "quickfix/Message.h"
#include "quickfix/SessionSettings.h"
#include "quickfix/Settings.h"

using namespace v8;
using namespace node;
using namespace FIX;

FixConnection::FixConnection(FIX::SessionSettings settings, std::string storeFactory): Nan::ObjectWrap()  {

  mSettings = settings;

	mFixApplication = new FixApplication(&mCallbacks, &mCallbackRegistry);

	setup(settings, storeFactory);

}

FixConnection::FixConnection(FixApplication* application, FIX::SessionSettings settings, std::string storeFactory): Nan::ObjectWrap()  {

  mSettings = settings;

	mFixApplication = application;
	mFixApplication->setCallbacks(&mCallbacks);
	mFixApplication->setCallbackRegistry(&mCallbackRegistry);

	setup(settings, storeFactory);

}

void FixConnection::setup(FIX::SessionSettings settings, std::string storeFactory) {
	#ifdef HAVE_POSTGRESQL
	if(storeFactory == "postgresql") {
		mStoreFactory = new FIX::PostgreSQLStoreFactory(mSettings);
		mLogFactory = new FIX::PostgreSQLLogFactory(mSettings);
	} else
	#endif
	#ifdef HAVE_MYSQL
	if (storeFactory == "mysql") {
		mStoreFactory = new FIX::MySQLStoreFactory(mSettings);
		mLogFactory = new FIX::MySQLLogFactory(mSettings);
	} else
	#endif
	#ifdef HAVE_ODBC
	if (storeFactory == "odbc") {
		mStoreFactory = new FIX::OdbcStoreFactory(mSettings);
		mLogFactory = new FIX::OdbcLogFactory(mSettings);
	} else
	#endif
	#if defined HAVE_POSTGRESQL || defined HAVE_MYSQL || defined HAVE_ODBC
	{
		mStoreFactory = new FIX::FileStoreFactory(mSettings);
		mLogFactory = new FIX::FileLogFactory(mSettings);
	}
	#else
	mStoreFactory = new FIX::FileStoreFactory(mSettings);
	mLogFactory = new FIX::FileLogFactory(mSettings);
	#endif
}

FixConnection::~FixConnection() {
}
