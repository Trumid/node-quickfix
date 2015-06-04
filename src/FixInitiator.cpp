
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

//Persistent<Function> FixInitiator::constructor;


/*
 * Node API
 */

void FixInitiator::Initialize(Handle<Object> target) {
  NanScope();

  Local<FunctionTemplate> ctor = NanNew<FunctionTemplate>(FixInitiator::New);

  // TODO:: Figure out what the compile error is with this
  //NanAssignPersistent(constructor, ctor);

  ctor->InstanceTemplate()->SetInternalFieldCount(1);
  ctor->SetClassName(NanNew("FixInitiator"));

  NODE_SET_PROTOTYPE_METHOD(ctor, "start", start);
  NODE_SET_PROTOTYPE_METHOD(ctor, "send", send);
  NODE_SET_PROTOTYPE_METHOD(ctor, "sendRaw", sendRaw);
  NODE_SET_PROTOTYPE_METHOD(ctor, "stop", stop);
  NODE_SET_PROTOTYPE_METHOD(ctor, "isLoggedOn", isLoggedOn);
  NODE_SET_PROTOTYPE_METHOD(ctor, "getSessions", getSessions);
  NODE_SET_PROTOTYPE_METHOD(ctor, "getSession", getSession);

  target->Set(NanNew("FixInitiator"), ctor->GetFunction());
}

NAN_METHOD(FixInitiator::New) {
	NanScope();

	bool hasOptions = false;
	Local<Object> options;
	FixInitiator *initiator = NULL;

	if(!(args[1]->IsUndefined() || args[1]->IsNull())){
		hasOptions = true;
		options = NanNew( args[1]->ToObject() );
	}

	String::Utf8Value propertiesFile(args[0]);

	FIX::SessionSettings sessionSettings;

	if ( ! hasOptions) return NanThrowError("FixInitiator requires an options parameter");

	Local<String> propertiesFileKey =  NanNew<String>("propertiesFile");
	Local<String> settingsKey =  NanNew<String>("settings");

	if ( ! options->Has(propertiesFileKey) && ! options->Has(settingsKey)) return NanThrowError("you must provide FixInitiator either an options.settings string or options.propertiesFile path to a properties file");

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
		initiator = new FixInitiator(sessionSettings, std::string(*value));
	} else {
		initiator = new FixInitiator(sessionSettings, "file");
	}

	if (args.IsConstructCall()) {
		initiator->Wrap(args.This());
	} 

	Local<Object> callbackObj = NanNew( args[0]->ToObject() );
	NanAssignPersistent(initiator->mCallbacks, callbackObj);

	Local<Array> callbackNames = callbackObj->GetOwnPropertyNames();
	for (uint32_t i=0 ; i < callbackNames->Length() ; ++i) {
	  String::Utf8Value callbackName(callbackNames->Get(i)->ToString());
	  initiator->mCallbackRegistry.insert(*callbackName);
	}

	if(hasOptions){
		Local<String> credentialsKey =  NanNew<String>("credentials");
		if(options->Has(credentialsKey)){
			Local<Object> creds = options->Get(credentialsKey)->ToObject();
			fix_credentials* credentials = new fix_credentials;
			String::Utf8Value usernameStr(creds->Get(NanNew<String>("username"))->ToString());
			String::Utf8Value passwordStr(creds->Get(NanNew<String>("password"))->ToString());
			credentials->username = std::string(*usernameStr);
			credentials->password = std::string(*passwordStr);
			initiator->mFixApplication->setCredentials(credentials);
		}
	}

	NanReturnValue(args.This());
}

NAN_METHOD(FixInitiator::start) {
	NanScope();

	FixInitiator* instance = ObjectWrap::Unwrap<FixInitiator>(args.This());

	NanCallback *callback = new NanCallback(args[0].As<Function>());

	NanAsyncQueueWorker(new FixInitiatorStartWorker(callback, instance->mInitiator));

	NanReturnUndefined();
}

NAN_METHOD(FixInitiator::send) {
	NanScope();

	Local<Object> message = args[0]->ToObject();

	FIX::Message* fixMessage = new FIX::Message();
	FixMessageUtil::js2Fix(fixMessage, message);

	sendAsync(args, fixMessage);

	NanReturnUndefined();
}

NAN_METHOD(FixInitiator::sendRaw) {
	NanScope();

	String::Utf8Value message(args[0]->ToString());
  
	FIX::Message* fixMessage  = new FIX::Message(std::string(* message));

	sendAsync(args, fixMessage);

	NanReturnUndefined();
}

void FixInitiator::sendAsync(_NAN_METHOD_ARGS, FIX::Message* fixMessage) {
	FixInitiator* instance = ObjectWrap::Unwrap<FixInitiator>(args.This());
	NanCallback *callback = new NanCallback(args[1].As<Function>());

	NanAsyncQueueWorker(new FixSendWorker(callback, fixMessage));
}

NAN_METHOD(FixInitiator::stop) {
	NanScope();
	FixInitiator* instance = ObjectWrap::Unwrap<FixInitiator>(args.This());

	NanCallback *callback = new NanCallback(args[0].As<Function>());

	NanAsyncQueueWorker(new FixInitiatorStopWorker(callback, instance->mInitiator));

	NanReturnUndefined();
}

NAN_METHOD(FixInitiator::isLoggedOn) {
	NanScope();
	FixInitiator* instance = ObjectWrap::Unwrap<FixInitiator>(args.This());

	bool loggedOn = instance->mInitiator->isLoggedOn();

	NanReturnValue(loggedOn ? NanTrue() : NanFalse());
}

NAN_METHOD(FixInitiator::getSessions) {
	NanScope();
	FixInitiator* instance = ObjectWrap::Unwrap<FixInitiator>(args.This());

	std::set<FIX::SessionID> sessions = instance->mInitiator->getSessions();

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

NAN_METHOD(FixInitiator::getSession) {
	NanScope();
	FixInitiator* instance = ObjectWrap::Unwrap<FixInitiator>(args.This());

	Local<Object> sessionId = args[0]->ToObject();

	FIX::Session* session = instance->mInitiator->getSession(FixMessageUtil::jsToSessionId(sessionId));
	FixSession* fixSession = new FixSession();
	fixSession->setSession(session);

	Handle<Object> jsSession = FixSession::wrapFixSession(fixSession);

	NanReturnValue(jsSession);
}

FixInitiator::FixInitiator(FIX::SessionSettings settings, std::string storeFactory): FixConnection(settings, storeFactory) {
	mInitiator = new FIX::SocketInitiator(*mFixApplication, *mStoreFactory, mSettings, *mLogFactory);
}

FixInitiator::~FixInitiator() {
}



