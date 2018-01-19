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
#include "quickfix/ThreadedSocketAcceptor.h"
#ifdef HAVE_SSL
#include "quickfix/ThreadedSSLSocketAcceptor.h"
#endif

using namespace v8;
using namespace node;

class FixAcceptor : public FixConnection {
	public:
  	static NAN_MODULE_INIT(Init);

		static NAN_METHOD(New);
		static NAN_METHOD(start);
		static NAN_METHOD(send);
		static NAN_METHOD(sendRaw);
		static NAN_METHOD(stop);
		static NAN_METHOD(getSessions);
		static NAN_METHOD(getSession);

		FixAcceptor(FIX::SessionSettings settings, std::string storeFactory, bool ssl = false);
		FixAcceptor(FixApplication* application, FIX::SessionSettings settings, std::string storeFactory, bool ssl = false);
		
	private:
		~FixAcceptor();

		static Nan::Persistent<v8::Function> constructor;
		static void sendAsync(const Nan::FunctionCallbackInfo<v8::Value>& info, FIX::Message* message);
		FIX::Acceptor* mAcceptor;
		FixLoginProvider* mFixLoginProvider;
};

#endif /* FIXACCEPTOR_H_ */
