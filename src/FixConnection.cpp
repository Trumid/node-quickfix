/*
 * FixConnection.cpp
 *
 *  Created on: Sep 9, 2014
 *      Author: kdeol
 */

#include "FixConnection.h"

FixConnection::FixConnection(const char* propertiesFile): ObjectWrap()  {
	mSettings = FIX::SessionSettings(propertiesFile);
	mFixApplication = new FixApplication(&mAsyncFIXEvent, &mCallbacks);
	mStoreFactory = new FIX::FileStoreFactory(mSettings);
	mLogFactory = new FIX::FileLogFactory(mSettings);
}

FixConnection::~FixConnection() {
	uv_close((uv_handle_t*) &mAsyncFIXEvent, NULL);
}

