/*
 * FixAcceptor.h
 *
 *  Created on: Jul 28, 2014
 *      Author: kdeol
 */

#ifndef FIXACCEPTOR_H_
#define FIXACCEPTOR_H_

#include <v8.h>
#include <node.h>
#include <nan.h>
#include "FixConnection.h"
#include "FixLoginProvider.h"
#include "quickfix/SocketAcceptor.h"

using namespace v8;
using namespace node;

class FixAcceptor : public FixConnection {
	public:
		//static Persistent<Function> constructor;
		static void Initialize(Handle<Object> target);
		static NAN_METHOD(New);
		static NAN_METHOD(start);
		static NAN_METHOD(send);
		static NAN_METHOD(stop);
		static NAN_METHOD(getSessions);
		static NAN_METHOD(getSession);

		FixAcceptor(const char* propertiesFile, std::string storeFactory);

	private:
		~FixAcceptor();
		FIX::SocketAcceptor* mAcceptor;
};

#endif /* FIXACCEPTOR_H_ */
