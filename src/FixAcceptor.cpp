/*
 * FixAcceptor.cpp
 *
 *  Created on: Jul 28, 2014
 *      Author: kdeol
 */

#include "FixAcceptor.h"
#include "FixMessageUtil.h"
#include "FixAcceptorStartWorker.h"
#include "FixSendWorker.h"
#include "FixAcceptorStopWorker.h"

//Persistent<Function> FixAcceptor::constructor;

void FixAcceptor::Initialize(Handle<Object> target) {
  NanScope();

  Local<FunctionTemplate> ctor = NanNew<FunctionTemplate>(FixAcceptor::New);
  ctor->InstanceTemplate()->SetInternalFieldCount(1);
  ctor->SetClassName(NanNew("FixAcceptor"));

  NODE_SET_PROTOTYPE_METHOD(ctor, "start", start);
  NODE_SET_PROTOTYPE_METHOD(ctor, "send", send);
  NODE_SET_PROTOTYPE_METHOD(ctor, "stop", stop);

  target->Set(NanNew("FixAcceptor"), ctor->GetFunction());
}

NAN_METHOD(FixAcceptor::New) {
	NanScope();

	String::Utf8Value propertiesFile(args[0]);
	FixAcceptor *acceptor = new FixAcceptor(*propertiesFile);

	acceptor->Wrap(args.This());
	acceptor->mCallbacks = Persistent<Object>::New( args[1]->ToObject() );
	if(!(args[2]->IsUndefined() || args[2]->IsNull())){
		acceptor->mFixLoginProvider = ObjectWrap::Unwrap<FixLoginProvider>(Local<Object>::New( args[2]->ToObject()));
		acceptor->mFixApplication->setLogonProvider(acceptor->mFixLoginProvider);
		uv_async_init(uv_default_loop(), &acceptor->mAsyncLogonEvent, handleLogonEvent);
	}

	uv_async_init(uv_default_loop(), &acceptor->mAsyncFIXEvent, FixMessageUtil::handleFixEvent);

	NanReturnValue(args.This());
}

NAN_METHOD(FixAcceptor::start) {
	NanScope();

	FixAcceptor* instance = ObjectWrap::Unwrap<FixAcceptor>(args.This());

	NanCallback *callback = new NanCallback(args[0].As<Function>());

	NanAsyncQueueWorker(new FixAcceptorStartWorker(callback, instance->mAcceptor));

	NanReturnUndefined();
}

NAN_METHOD(FixAcceptor::send) {
	NanScope();

	FixAcceptor* instance = ObjectWrap::Unwrap<FixAcceptor>(args.This());
	Local<Object> message = args[0]->ToObject();
	NanCallback *callback = new NanCallback(args[1].As<Function>());

	FIX::Message* fixMessage = new FIX::Message();
	FixMessageUtil::js2Fix(fixMessage, message);

	FIX::SessionID senderSessionId = *(instance->mAcceptor->getSessions().begin());
	std::string senderId = senderSessionId.getSenderCompID().getString();
	fixMessage->getHeader().setField(49, senderId);

	NanAsyncQueueWorker(new FixSendWorker(callback, fixMessage));

	NanReturnUndefined();
}

NAN_METHOD(FixAcceptor::stop) {
	NanScope();
	FixAcceptor* instance = ObjectWrap::Unwrap<FixAcceptor>(args.This());

	NanCallback *callback = new NanCallback(args[0].As<Function>());

	NanAsyncQueueWorker(new FixAcceptorStopWorker(callback, instance->mAcceptor));

	NanReturnUndefined();
}

FixAcceptor::FixAcceptor(const char* propertiesFile): FixConnection(propertiesFile) {
	mAcceptor = new FIX::SocketAcceptor(*mFixApplication, *mStoreFactory, mSettings);
}

FixAcceptor::~FixAcceptor() {
}


