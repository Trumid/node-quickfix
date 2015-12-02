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
  Nan::HandleScope scope;

  Local<FunctionTemplate> ctor = Nan::New<FunctionTemplate>(FixAcceptor::New);
  ctor->InstanceTemplate()->SetInternalFieldCount(1);
  ctor->SetClassName(Nan::New("FixAcceptor").ToLocalChecked());

  Nan::SetPrototypeMethod(ctor, "start", start);
  Nan::SetPrototypeMethod(ctor, "send", send);
  Nan::SetPrototypeMethod(ctor, "sendRaw", sendRaw);
  Nan::SetPrototypeMethod(ctor, "stop", stop);
  Nan::SetPrototypeMethod(ctor, "getSessions", getSessions);
  Nan::SetPrototypeMethod(ctor, "getSession", getSession);

  target->Set(Nan::New("FixAcceptor").ToLocalChecked(), ctor->GetFunction());
}

NAN_METHOD(FixAcceptor::New) {
	Nan::HandleScope scope;

	bool hasOptions = false;
	Local<Object> options;
	FixAcceptor *acceptor = NULL;

	if(!(info[1]->IsUndefined() || info[1]->IsNull())){
		hasOptions = true;
		options = info[1]->ToObject();
	}

	FIX::SessionSettings sessionSettings;

	if ( ! hasOptions) return Nan::ThrowError("FixAcceptor requires an options parameter");

	Local<String> propertiesFileKey =  Nan::New<String>("propertiesFile").ToLocalChecked();
	Local<String> settingsKey =  Nan::New<String>("settings").ToLocalChecked();

	if ( ! options->Has(propertiesFileKey) && ! options->Has(settingsKey)) return Nan::ThrowError("you must provide FixAcceptor either an options.settings string or options.propertiesFile path to a properties file");

	if (options->Has(propertiesFileKey)){
		String::Utf8Value propertiesFile(options->Get(Nan::New<String>("propertiesFile").ToLocalChecked())->ToString());
		sessionSettings = FIX::SessionSettings(*propertiesFile);
	} else if (options->Has(settingsKey)){
		String::Utf8Value settings(options->Get(Nan::New<String>("settings").ToLocalChecked())->ToString());
		stringstream stream;
		stream << *settings;
		sessionSettings = FIX::SessionSettings(stream);
	}

	Local<String> storeFactoryKey =  Nan::New<String>("storeFactory").ToLocalChecked();

	if(options->Has(storeFactoryKey)) {
		String::Utf8Value value(options->Get(storeFactoryKey)->ToString());
		acceptor = new FixAcceptor(sessionSettings, std::string(*value));
	} else {
		acceptor = new FixAcceptor(sessionSettings, "file");
	}

	acceptor->Wrap(info.This());
	Local<Object> callbackObj = info[0]->ToObject();
	acceptor->mCallbacks.Reset(callbackObj);

	Local<Array> callbackNames = callbackObj->GetOwnPropertyNames();
	for (uint32_t i=0 ; i < callbackNames->Length() ; ++i) {
	  String::Utf8Value callbackName(callbackNames->Get(i)->ToString());
	  acceptor->mCallbackRegistry.insert(*callbackName);
	}

	if(hasOptions){
		Local<String> logonProviderKey =  Nan::New<String>("logonProvider").ToLocalChecked();
		if(options->Has(logonProviderKey)) {
			acceptor->mFixLoginProvider = Nan::ObjectWrap::Unwrap<FixLoginProvider>(options->Get(logonProviderKey)->ToObject());
			acceptor->mFixApplication->setLogonProvider(acceptor->mFixLoginProvider);
		}

		Local<String> credentialsKey =  Nan::New<String>("credentials").ToLocalChecked();

		if(options->Has(credentialsKey)){
			Local<Object> creds = options->Get(credentialsKey)->ToObject();
			fix_credentials* credentials = new fix_credentials;
			String::Utf8Value usernameStr(creds->Get(Nan::New<String>("username").ToLocalChecked())->ToString());
			String::Utf8Value passwordStr(creds->Get(Nan::New<String>("password").ToLocalChecked())->ToString());
			credentials->username = std::string(*usernameStr);
			credentials->password = std::string(*passwordStr);
			acceptor->mFixApplication->setCredentials(credentials);
		}
	}

	info.GetReturnValue().Set(info.This());
}

NAN_METHOD(FixAcceptor::start) {
	Nan::HandleScope scope;
	FixAcceptor* instance = Nan::ObjectWrap::Unwrap<FixAcceptor>(info.Holder());
	Nan::Callback *callback = new Nan::Callback(info[0].As<Function>());
	Nan::AsyncQueueWorker(new FixAcceptorStartWorker(callback, instance->mAcceptor));
	return;
}

NAN_METHOD(FixAcceptor::send) {
	Nan::HandleScope scope;

	Local<Object> message = info[0]->ToObject();
	FIX::Message* fixMessage = new FIX::Message();
	FixMessageUtil::js2Fix(fixMessage, message);

	sendAsync(info, fixMessage);

	return;
}

NAN_METHOD(FixAcceptor::sendRaw) {
	Nan::HandleScope scope;

	String::Utf8Value message(info[0]->ToString());

	FIX::Message* fixMessage = new FIX::Message(std::string(* message));

	sendAsync(info, fixMessage);

	return;
}

void FixAcceptor::sendAsync(const Nan::FunctionCallbackInfo<v8::Value>& info, FIX::Message* fixMessage) {
	FixAcceptor* instance = Nan::ObjectWrap::Unwrap<FixAcceptor>(info.Holder());
	Nan::Callback *callback = new Nan::Callback(info[1].template As<Function>());

	if(!fixMessage->getHeader().isSetField(FIELD::SenderCompID)) {
		std::set<FIX::SessionID>::iterator it;
		std::set<FIX::SessionID> sessions = instance->mAcceptor->getSessions();
		std::string targetCompId = fixMessage->getHeader().getField(FIELD::TargetCompID);

		for(it = sessions.begin(); it != sessions.end(); ++it ){
			if(targetCompId.compare(it->getTargetCompID()) == 0) {
				fixMessage->setSessionID(*it);
				break;
			}
		}
	}

	Nan::AsyncQueueWorker(new FixSendWorker(callback, fixMessage));
}

NAN_METHOD(FixAcceptor::stop) {
	Nan::HandleScope scope;
	FixAcceptor* instance = Nan::ObjectWrap::Unwrap<FixAcceptor>(info.Holder());
	Nan::Callback *callback = new Nan::Callback(info[0].As<Function>());

	Nan::AsyncQueueWorker(new FixAcceptorStopWorker(callback, instance->mAcceptor));
	return;
}

NAN_METHOD(FixAcceptor::getSessions) {
	Nan::HandleScope scope;
	FixAcceptor* instance = Nan::ObjectWrap::Unwrap<FixAcceptor>(info.Holder());
	std::set<FIX::SessionID> sessions = instance->mAcceptor->getSessions();

	Local<Array> sessionsArr = Nan::New<Array>(sessions.size());
	std::set<FIX::SessionID>::iterator it;
	int i = 0;
	for(it = sessions.begin(); it != sessions.end(); ++it ){
		FIX::SessionID id = *it;
		sessionsArr->Set(i, FixMessageUtil::sessionIdToJs(&id));
		i++;
	}
	info.GetReturnValue().Set(sessionsArr);
}

NAN_METHOD(FixAcceptor::getSession) {
	Nan::HandleScope scope;
	FixAcceptor* instance = Nan::ObjectWrap::Unwrap<FixAcceptor>(info.Holder());
	Local<Object> sessionId = info[0]->ToObject();

	FIX::Session* session = instance->mAcceptor->getSession(FixMessageUtil::jsToSessionId(sessionId));

	FixSession* fixSession = new FixSession();
	fixSession->setSession(session);

	Handle<Object> jsSession = FixSession::wrapFixSession(fixSession);
	info.GetReturnValue().Set(jsSession);
}
