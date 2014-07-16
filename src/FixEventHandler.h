/*
 * FixCallbackHandler.h
 *
 *  Created on: Jul 14, 2014
 *      Author: kdeol
 */

#ifndef FIXEVENTHANDLER_H_
#define FIXEVENTHANDLER_H_
#include <node.h>

using namespace v8;

class FixEventHandler {
	public:
		FixEventHandler();
		FixEventHandler(const Arguments& args);
		virtual ~FixEventHandler();
		static void handleFixEvent(uv_async_t *handle, int status);
		Persistent<Object> getApp();
	private:
		Persistent<Object> mApp;
};

#endif /* FIXCALLBACKHANDLER_H_ */
