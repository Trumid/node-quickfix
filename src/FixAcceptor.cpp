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
#include "FixCredentials.h"
#include "FixSession.h"

//Persistent<Function> FixAcceptor::constructor;

void FixAcceptor::Initialize(Handle<Object> target) {
  NanScope();

  Local<FunctionTemplate> ctor = NanNew<FunctionTemplate>(FixAcceptor::New);
  ctor->InstanceTemplate()->SetInternalFieldCount(1);
  ctor->SetClassName(NanNew("FixAcceptor"));

  NODE_SET_PROTOTYPE_METHOD(ctor, "start", start);
  NODE_SET_PROTOTYPE_METHOD(ctor, "send", send);
  NODE_SET_PROTOTYPE_METHOD(ctor, "stop", stop);
  NODE_SET_PROTOTYPE_METHOD(ctor, "getSessions", getSessions);
  NODE_SET_PROTOTYPE_METHOD(ctor, "getSession", getSession);

  target->Set(NanNew("FixAcceptor"), ctor->GetFunction());
}

NAN_METHOD(FixAcceptor::New) {
	NanScope();
	bool hasOptions = false;
	Local<Object> options;
	FixAcceptor *acceptor = NULL;

	if(!(args[2]->IsUndefined() || args[2]->IsNull())){
		hasOptions = true;
		options = Local<Object>::New( args[2]->ToObject() );
	}

	String::Utf8Value propertiesFile(args[0]);

	if(hasOptions) {
		Local<String> storeFactoryKey =  NanNew<String>("storeFactory");
		if(options->Has(storeFactoryKey)) {
			String::Utf8Value value(options->Get(storeFactoryKey)->ToString());
			acceptor = new FixAcceptor(*propertiesFile, std::string(*value));
		} else {
			acceptor = new FixAcceptor(*propertiesFile, "file");
		}
	} else {
		acceptor = new FixAcceptor(*propertiesFile, "file");
	}

	acceptor->Wrap(args.This());
	acceptor->mCallbacks = Persistent<Object>::New( args[1]->ToObject() );
	if(hasOptions){
		Local<String> logonProviderKey =  NanNew<String>("logonProvider");
		if(options->Has(logonProviderKey)) {
			acceptor->mFixLoginProvider = ObjectWrap::Unwrap<FixLoginProvider>(Local<Object>::New(options->Get(logonProviderKey)->ToObject()));
			acceptor->mFixApplication->setLogonProvider(acceptor->mFixLoginProvider);
			uv_async_init(uv_default_loop(), &acceptor->mAsyncLogonEvent, handleLogonEvent);
		}

		Local<String> credentialsKey =  NanNew<String>("credentials");
		if(options->Has(credentialsKey)){
			Local<Object> creds = options->Get(credentialsKey)->ToObject();
			fix_credentials* credentials = new fix_credentials;
			String::Utf8Value usernameStr(creds->Get(NanNew<String>("username"))->ToString());
			String::Utf8Value passwordStr(creds->Get(NanNew<String>("password"))->ToString());
			credentials->username = std::string(*usernameStr);
			credentials->password = std::string(*passwordStr);
			acceptor->mFixApplication->setCredentials(credentials);
		}
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

NAN_METHOD(FixAcceptor::getSessions) {
	NanScope();
	FixAcceptor* instance = ObjectWrap::Unwrap<FixAcceptor>(args.This());

	std::set<FIX::SessionID> sessions = instance->mAcceptor->getSessions();

	Local<Array> sessionsArr = Array::New(sessions.size());
	std::set<FIX::SessionID>::iterator it;
	int i = 0;
	for(it = sessions.begin(); it != sessions.end(); ++it ){
		FIX::SessionID id = *it;
		sessionsArr->Set(i, FixMessageUtil::sessionIdToJs(&id));
		i++;
	}

	NanReturnValue(sessionsArr);
}

NAN_METHOD(FixAcceptor::getSession) {
	NanScope();
	FixAcceptor* instance = ObjectWrap::Unwrap<FixAcceptor>(args.This());

	Local<Object> sessionId = args[0]->ToObject();

	FIX::Session* session = instance->mAcceptor->getSession(FixMessageUtil::jsToSessionId(sessionId));
	FixSession* fixSession = new FixSession();
	fixSession->setSession(session);

	Handle<Object> jsSession = FixSession::wrapFixSession(fixSession);

	NanReturnValue(jsSession);
}

FixAcceptor::FixAcceptor(const char* propertiesFile, std::string storeFactory): FixConnection(propertiesFile, storeFactory) {
	mAcceptor = new FIX::ThreadedSocketAcceptor(*mFixApplication, *mStoreFactory, mSettings, *mLogFactory);
}

FixAcceptor::~FixAcceptor() {
}


