
#include "quickfix/FileStore.h"
#include "quickfix/FileLog.h"
#include "quickfix/Initiator.h"
#include "quickfix/SocketInitiator.h"
#include "quickfix/Session.h"
#include "quickfix/SessionID.h"
#include "quickfix/SessionSettings.h"

#include "FixInitiator.h"
#include "FixApplication.h"
#include "FixEvent.h"
#include "FixSession.h"
#include "FixLoginProvider.h"

#include "FixInitiatorStartWorker.h"
#include "FixSendWorker.h"
#include "FixInitiatorStopWorker.h"
#include "FixMessageUtil.h"
#include <iostream>
#include <sstream>
#include <unordered_set>

using namespace std;

//#include "closure.h"

//Nan::Persistent<Function> FixInitiator::constructor;


/*
 * Node API
 */

void FixInitiator::Initialize(Handle<Object> target) {
	Nan::HandleScope scope;

	Local<FunctionTemplate> ctor = Nan::New<FunctionTemplate>(FixInitiator::New);

	// TODO:: Figure out what the compile error is with this
	//constructor.Reset(ctor);

	ctor->InstanceTemplate()->SetInternalFieldCount(1);
	ctor->SetClassName(Nan::New("FixInitiator").ToLocalChecked());

	Nan::SetPrototypeMethod(ctor, "start", start);
	Nan::SetPrototypeMethod(ctor, "send", send);
	Nan::SetPrototypeMethod(ctor, "sendRaw", sendRaw);
	Nan::SetPrototypeMethod(ctor, "stop", stop);
	Nan::SetPrototypeMethod(ctor, "isLoggedOn", isLoggedOn);
	Nan::SetPrototypeMethod(ctor, "getSessions", getSessions);
	Nan::SetPrototypeMethod(ctor, "getSession", getSession);

	target->Set(Nan::New("FixInitiator").ToLocalChecked(), ctor->GetFunction());
}

NAN_METHOD(FixInitiator::New) {
	Nan::HandleScope scope;

	bool hasOptions = false;
	Local<Object> options;
	FixInitiator *initiator = NULL;

	if(!(info[1]->IsUndefined() || info[1]->IsNull())){
		hasOptions = true;
		options = info[1]->ToObject();
	}

	String::Utf8Value propertiesFile(info[0]);

	FIX::SessionSettings sessionSettings;

	if ( ! hasOptions) return Nan::ThrowError("FixInitiator requires an options parameter");

	Local<String> propertiesFileKey =  Nan::New<String>("propertiesFile").ToLocalChecked();
	Local<String> settingsKey =  Nan::New<String>("settings").ToLocalChecked();

	if ( ! options->Has(propertiesFileKey) && ! options->Has(settingsKey)) return Nan::ThrowError("you must provide FixInitiator either an options.settings string or options.propertiesFile path to a properties file");

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
		initiator = new FixInitiator(sessionSettings, std::string(*value));
	} else {
		initiator = new FixInitiator(sessionSettings, "file");
	}

	if (info.IsConstructCall()) {
		initiator->Wrap(info.This());
	}

	Local<Object> callbackObj = info[0]->ToObject();
	initiator->mCallbacks.Reset(callbackObj);

	Local<Array> callbackNames = callbackObj->GetOwnPropertyNames();
	for (uint32_t i=0 ; i < callbackNames->Length() ; ++i) {
	  String::Utf8Value callbackName(callbackNames->Get(i)->ToString());
	  initiator->mCallbackRegistry.insert(*callbackName);
	}

	if(hasOptions){
		Local<String> credentialsKey =  Nan::New<String>("credentials").ToLocalChecked();
		if(options->Has(credentialsKey)){
			Local<Object> creds = options->Get(credentialsKey)->ToObject();
			fix_credentials* credentials = new fix_credentials;
			String::Utf8Value usernameStr(creds->Get(Nan::New<String>("username").ToLocalChecked())->ToString());
			String::Utf8Value passwordStr(creds->Get(Nan::New<String>("password").ToLocalChecked())->ToString());
			credentials->username = std::string(*usernameStr);
			credentials->password = std::string(*passwordStr);
			initiator->mFixApplication->setCredentials(credentials);
		}
	}

	info.GetReturnValue().Set(info.This());
}

NAN_METHOD(FixInitiator::start) {
	Nan::HandleScope scope;

	FixInitiator* instance = Nan::ObjectWrap::Unwrap<FixInitiator>(info.This());

	Nan::Callback *callback = new Nan::Callback(info[0].As<Function>());

	Nan::AsyncQueueWorker(new FixInitiatorStartWorker(callback, instance->mInitiator));

	return;
}

NAN_METHOD(FixInitiator::send) {
	Nan::HandleScope scope;

	Local<Object> message = info[0]->ToObject();

	FIX::Message* fixMessage = new FIX::Message();
	FixMessageUtil::js2Fix(fixMessage, message);

	sendAsync(info, fixMessage);

	return;
}

NAN_METHOD(FixInitiator::sendRaw) {
	Nan::HandleScope scope;

	String::Utf8Value message(info[0]->ToString());

	FIX::Message* fixMessage  = new FIX::Message(std::string(* message));

	sendAsync(info, fixMessage);

	return;
}

void FixInitiator::sendAsync(const Nan::FunctionCallbackInfo<v8::Value>& info, FIX::Message* fixMessage) {
	FixInitiator* instance = Nan::ObjectWrap::Unwrap<FixInitiator>(info.This());

	Nan::Callback *callback = new Nan::Callback(info[1].As<Function>());

	Nan::AsyncQueueWorker(new FixSendWorker(callback, fixMessage));
}

NAN_METHOD(FixInitiator::stop) {
	Nan::HandleScope scope;
	FixInitiator* instance = Nan::ObjectWrap::Unwrap<FixInitiator>(info.This());

	Nan::Callback *callback = new Nan::Callback(info[0].As<Function>());

	Nan::AsyncQueueWorker(new FixInitiatorStopWorker(callback, instance->mInitiator));

	return;
}

NAN_METHOD(FixInitiator::isLoggedOn) {
	Nan::HandleScope scope;
	FixInitiator* instance = Nan::ObjectWrap::Unwrap<FixInitiator>(info.This());

	bool loggedOn = instance->mInitiator->isLoggedOn();

	info.GetReturnValue().Set(loggedOn ? Nan::True() : Nan::False());
}

NAN_METHOD(FixInitiator::getSessions) {
	Nan::HandleScope scope;
	FixInitiator* instance = Nan::ObjectWrap::Unwrap<FixInitiator>(info.This());

	std::set<FIX::SessionID> sessions = instance->mInitiator->getSessions();

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

NAN_METHOD(FixInitiator::getSession) {
	Nan::HandleScope scope;
	FixInitiator* instance = ObjectWrap::Unwrap<FixInitiator>(info.This());

	Local<Object> sessionId = info[0]->ToObject();

	FIX::Session* session = instance->mInitiator->getSession(FixMessageUtil::jsToSessionId(sessionId));

	Handle<Object> jsSession(FixSession::wrapFixSession(session));

	info.GetReturnValue().Set(jsSession);
}

FixInitiator::FixInitiator(FIX::SessionSettings settings, std::string storeFactory): FixConnection(settings, storeFactory) {
	mInitiator = new FIX::SocketInitiator(*mFixApplication, *mStoreFactory, mSettings, *mLogFactory);
}

FixInitiator::~FixInitiator() {
}
