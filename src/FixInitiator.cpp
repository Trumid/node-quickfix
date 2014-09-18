
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

#include "FixInitiatorStartWorker.h"
#include "FixSendWorker.h"
#include "FixInitiatorStopWorker.h"
#include "FixMessageUtil.h"

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
  NODE_SET_PROTOTYPE_METHOD(ctor, "stop", stop);
  NODE_SET_PROTOTYPE_METHOD(ctor, "isLoggedOn", isLoggedOn);
  NODE_SET_PROTOTYPE_METHOD(ctor, "getSessions", getSessions);
  NODE_SET_PROTOTYPE_METHOD(ctor, "getSession", getSession);

  target->Set(NanNew("FixInitiator"), ctor->GetFunction());
}

NAN_METHOD(FixInitiator::New) {
	NanScope();

	String::Utf8Value propertiesFile(args[0]);
	FixInitiator *initiator = new FixInitiator(*propertiesFile);

	initiator->Wrap(args.This());
	initiator->mCallbacks = Persistent<Object>::New( args[1]->ToObject() );

	uv_async_init(uv_default_loop(), &initiator->mAsyncFIXEvent, FixMessageUtil::handleFixEvent);

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

	FixInitiator* instance = ObjectWrap::Unwrap<FixInitiator>(args.This());

	Local<Object> message = args[0]->ToObject();
	NanCallback *callback = new NanCallback(args[1].As<Function>());

	FIX::Message* fixMessage = new FIX::Message();
	FixMessageUtil::js2Fix(fixMessage, message);

	NanAsyncQueueWorker(new FixSendWorker(callback, fixMessage));

	NanReturnUndefined();
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

	Local<Array> sessionsArr = Array::New(sessions.size());
	std::set<FIX::SessionID>::iterator it;
	int i = 0;
	for(it = sessions.begin(); it != sessions.end(); ++it ){
		FIX::SessionID id = *it;
		sessionsArr->Set(i, FixMessageUtil::sessionIdToJs(&id));
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

	Handle<Object> jsSession = Object::New();
	FixSession::Initialize(jsSession);
	jsSession = FixSession::wrapFixSession(fixSession, jsSession);

	NanReturnValue(jsSession);
}

FixInitiator::FixInitiator(const char* propertiesFile): FixConnection(propertiesFile) {
	mInitiator = new FIX::SocketInitiator(*mFixApplication, *mStoreFactory, mSettings);
}

FixInitiator::~FixInitiator() {
}



