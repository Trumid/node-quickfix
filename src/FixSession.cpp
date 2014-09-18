/*
 * FixSession.cpp
 *
 *  Created on: Sep 16, 2014
 *      Author: kdeol
 */

#include "FixSession.h"
#include "FixMessageUtil.h"

class FixSessionAsyncWorker : public NanAsyncWorker {
	public:
		FixSessionAsyncWorker(NanCallback *callback, FIX::Session* session)
			: NanAsyncWorker(callback), session(session) {}
		~FixSessionAsyncWorker() {}

		void HandleOKCallback () {
			NanScope();

			Local<Value> argv[] = {
				NanNull()
			};

			callback->Call(1, argv);
		};
	protected:
		FIX::Session* session;
};

class FixSessionDisconnectWorker : public FixSessionAsyncWorker {
	public:
		FixSessionDisconnectWorker(NanCallback *callback, FIX::Session* session)
				: FixSessionAsyncWorker(callback, session) {}
		~FixSessionDisconnectWorker() {}

		void Execute () {
			session->disconnect();
		};
};

class FixSessionLogonWorker : public FixSessionAsyncWorker {
	public:
		FixSessionLogonWorker(NanCallback *callback, FIX::Session* session)
				: FixSessionAsyncWorker(callback, session) {}
		~FixSessionLogonWorker() {}

		void Execute () {
			session->logon();
		};
};

class FixSessionLogoutWorker : public FixSessionAsyncWorker {
	public:
		FixSessionLogoutWorker(NanCallback *callback, FIX::Session* session)
				: FixSessionAsyncWorker(callback, session) {}
		~FixSessionLogoutWorker() {}

		void Execute () {
			session->logout();
		};
};

FixSession::FixSession() : ObjectWrap() {
}

FixSession::~FixSession() {
}

void FixSession::setSession(FIX::Session* session) {
	mSession = session;
}

void FixSession::Initialize(Handle<Object> target) {
	NanScope();

	Local<FunctionTemplate> ctor = NanNew<FunctionTemplate>(FixSession::New);

	ctor->InstanceTemplate()->SetInternalFieldCount(6);
	ctor->SetClassName(NanNew("FixSession"));

	node::SetPrototypeMethod(ctor, "isEnabled", isEnabled);
	node::SetPrototypeMethod(ctor, "logon", logon);
	node::SetPrototypeMethod(ctor, "logout", logout);
	node::SetPrototypeMethod(ctor, "isLoggedOn", isLoggedOn);
	node::SetPrototypeMethod(ctor, "getSessionID", getSessionID);
	node::SetPrototypeMethod(ctor, "disconnect", disconnect);

	target->Set(NanNew("FixSession"), ctor->GetFunction());
}

Handle<Object> FixSession::wrapFixSession(FixSession* fixSession, Handle<Object> handle) {
	NanScope();
	fixSession->Wrap(handle);
	return handle;
}

NAN_METHOD(FixSession::New) {
	NanScope();

	FixSession *fixSession = new FixSession();

	fixSession->Wrap(args.This());

	NanReturnValue(args.This());
}

NAN_METHOD(FixSession::isEnabled) {
	NanScope();

	FixSession* instance = ObjectWrap::Unwrap<FixSession>(args.This());
	bool isEnabled = instance->mSession->isEnabled();

	NanReturnValue(isEnabled ? NanTrue() : NanFalse());
}

NAN_METHOD(FixSession::isLoggedOn) {
	NanScope();

	FixSession* instance = ObjectWrap::Unwrap<FixSession>(args.This());
	bool isLoggedOn = instance->mSession->isLoggedOn();

	NanReturnValue(isLoggedOn ? NanTrue() : NanFalse());
}

NAN_METHOD(FixSession::getSessionID) {
	NanScope();

	FixSession* instance = ObjectWrap::Unwrap<FixSession>(args.This());
	FIX::SessionID sessionId = instance->mSession->getSessionID();
	Handle<Value> jsSessionId = FixMessageUtil::sessionIdToJs(&sessionId);

	NanReturnValue(jsSessionId);
}

NAN_METHOD(FixSession::disconnect) {
	NanScope();

	FixSession* instance = ObjectWrap::Unwrap<FixSession>(args.This());
	NanCallback *callback = new NanCallback(args[0].As<Function>());

	NanAsyncQueueWorker(new FixSessionDisconnectWorker(callback, instance->mSession));

	NanReturnUndefined();
}

NAN_METHOD(FixSession::logon) {
	NanScope();

	FixSession* instance = ObjectWrap::Unwrap<FixSession>(args.This());
	NanCallback *callback = new NanCallback(args[0].As<Function>());

	NanAsyncQueueWorker(new FixSessionLogonWorker(callback, instance->mSession));

	NanReturnUndefined();
}

NAN_METHOD(FixSession::logout) {
	NanScope();

	FixSession* instance = ObjectWrap::Unwrap<FixSession>(args.This());
	NanCallback *callback = new NanCallback(args[0].As<Function>());

	NanAsyncQueueWorker(new FixSessionLogoutWorker(callback, instance->mSession));

	NanReturnUndefined();
}
