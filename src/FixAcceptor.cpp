/*
 * FixAcceptor.cpp
 *
 *  Created on: Jul 28, 2014
 *      Author: kdeol
 */

#include "Dispatcher.h"
#include "FixAcceptor.h"
#include "FixMessageUtil.h"
#include "FixAcceptorStartWorker.h"
#include "FixSendWorker.h"
#include "FixAcceptorStopWorker.h"
#include "FixCredentials.h"
#include "FixSession.h"
#include "quickfix/Dictionary.h"
#include "quickfix/SessionSettings.h"
#include <iostream>
#include <sstream>
#include <unordered_set>

using namespace FIX;
using namespace std;

FixAcceptor::FixAcceptor(FIX::SessionSettings settings, std::string storeFactory): FixConnection(settings, storeFactory) {
	mAcceptor = new FIX::ThreadedSocketAcceptor(*mFixApplication, *mStoreFactory, mSettings, *mLogFactory);
}

FixAcceptor::FixAcceptor(FixApplication* application, FIX::SessionSettings settings, std::string storeFactory): FixConnection(application, settings, storeFactory) {
	mAcceptor = new FIX::ThreadedSocketAcceptor(*mFixApplication, *mStoreFactory, mSettings, *mLogFactory);
}

FixAcceptor::~FixAcceptor() {
}

void FixAcceptor::Initialize(Handle<Object> target) {
  NanScope();

  Local<FunctionTemplate> ctor = NanNew<FunctionTemplate>(FixAcceptor::New);
  ctor->InstanceTemplate()->SetInternalFieldCount(1);
  ctor->SetClassName(NanNew("FixAcceptor"));

  NODE_SET_PROTOTYPE_METHOD(ctor, "start", start);
  NODE_SET_PROTOTYPE_METHOD(ctor, "send", send);
  NODE_SET_PROTOTYPE_METHOD(ctor, "sendRaw", sendRaw);
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

	if(!(args[1]->IsUndefined() || args[1]->IsNull())){
		hasOptions = true;
		options = NanNew( args[1]->ToObject() );
	}

	FIX::SessionSettings sessionSettings;

	if ( ! hasOptions) return NanThrowError("FixAcceptor requires an options parameter");

	Local<String> propertiesFileKey =  NanNew<String>("propertiesFile");
	Local<String> settingsKey =  NanNew<String>("settings");

	if ( ! options->Has(propertiesFileKey) && ! options->Has(settingsKey)) return NanThrowError("you must provide FixAcceptor either an options.settings string or options.propertiesFile path to a properties file");

	if (options->Has(propertiesFileKey)){
		String::Utf8Value propertiesFile(options->Get(NanNew<String>("propertiesFile"))->ToString());
		sessionSettings = FIX::SessionSettings(*propertiesFile);
	} else if (options->Has(settingsKey)){
		String::Utf8Value settings(options->Get(NanNew<String>("settings"))->ToString());
		stringstream stream;
		stream << *settings;
		sessionSettings = FIX::SessionSettings(stream);
	}

	Local<String> storeFactoryKey =  NanNew<String>("storeFactory");

	if(options->Has(storeFactoryKey)) {
		String::Utf8Value value(options->Get(storeFactoryKey)->ToString());
		acceptor = new FixAcceptor(sessionSettings, std::string(*value));
	} else {
		acceptor = new FixAcceptor(sessionSettings, "file");
	}

	acceptor->Wrap(args.This());
	Local<Object> callbackObj = NanNew( args[0]->ToObject() );
	NanAssignPersistent(acceptor->mCallbacks, callbackObj);

	Local<Array> callbackNames = callbackObj->GetOwnPropertyNames();
	for (uint32_t i=0 ; i < callbackNames->Length() ; ++i) {
	  String::Utf8Value callbackName(callbackNames->Get(i)->ToString());
	  acceptor->mCallbackRegistry.insert(*callbackName);
	}

	if(hasOptions){
		Local<String> logonProviderKey =  NanNew<String>("logonProvider");
		if(options->Has(logonProviderKey)) {
			acceptor->mFixLoginProvider = ObjectWrap::Unwrap<FixLoginProvider>(NanNew(options->Get(logonProviderKey)->ToObject()));
			acceptor->mFixApplication->setLogonProvider(acceptor->mFixLoginProvider);
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

	NanReturnValue(args.This());
}

NAN_METHOD(FixAcceptor::start) {
	NanScope();
	FixAcceptor* instance = ObjectWrap::Unwrap<FixAcceptor>(args.Holder());
	NanCallback *callback = new NanCallback(args[0].As<Function>());
	NanAsyncQueueWorker(new FixAcceptorStartWorker(callback, instance->mAcceptor));
	NanReturnUndefined();
}

NAN_METHOD(FixAcceptor::send) {
	NanScope();

	Local<Object> message = args[0]->ToObject();
	FIX::Message* fixMessage = new FIX::Message();
	FixMessageUtil::js2Fix(fixMessage, message);

	sendAsync(args, fixMessage);

	NanReturnUndefined();
}

NAN_METHOD(FixAcceptor::sendRaw) {
	NanScope();

	String::Utf8Value message(args[0]->ToString());
  
	FIX::Message* fixMessage  = new FIX::Message(std::string(* message));

	sendAsync(args, fixMessage);

	NanReturnUndefined();
}

void FixAcceptor::sendAsync(_NAN_METHOD_ARGS, FIX::Message* fixMessage) {
	FixAcceptor* instance = ObjectWrap::Unwrap<FixAcceptor>(args.Holder());
	NanCallback *callback = new NanCallback(args[1].As<Function>());

	FIX::SessionID senderSessionId = *(instance->mAcceptor->getSessions().begin());
	std::string senderId = senderSessionId.getSenderCompID().getString();
	fixMessage->getHeader().setField(49, senderId);

	NanAsyncQueueWorker(new FixSendWorker(callback, fixMessage));
}

NAN_METHOD(FixAcceptor::stop) {
	NanScope();
	FixAcceptor* instance = ObjectWrap::Unwrap<FixAcceptor>(args.Holder());
	NanCallback *callback = new NanCallback(args[0].As<Function>());

	NanAsyncQueueWorker(new FixAcceptorStopWorker(callback, instance->mAcceptor));
	NanReturnUndefined();
}

NAN_METHOD(FixAcceptor::getSessions) {
	NanScope();
	FixAcceptor* instance = ObjectWrap::Unwrap<FixAcceptor>(args.Holder());
	std::set<FIX::SessionID> sessions = instance->mAcceptor->getSessions();

	Local<Array> sessionsArr = NanNew<Array>(sessions.size());
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
	FixAcceptor* instance = ObjectWrap::Unwrap<FixAcceptor>(args.Holder());
	Local<Object> sessionId = args[0]->ToObject();

	FIX::Session* session = instance->mAcceptor->getSession(FixMessageUtil::jsToSessionId(sessionId));

	FixSession* fixSession = new FixSession();
	fixSession->setSession(session);

	Handle<Object> jsSession = FixSession::wrapFixSession(fixSession);
	NanReturnValue(jsSession);
}
