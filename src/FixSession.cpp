/*
 * FixSession.cpp
 *
 *  Created on: Sep 16, 2014
 *      Author: kdeol
 */

#include "FixSession.h"
#include "FixMessageUtil.h"

Nan::Persistent<Function> g_Ctor;

class FixSessionAsyncWorker : public Nan::AsyncWorker {
	public:
		FixSessionAsyncWorker(Nan::Callback *callback, FIX::Session* session)
			: Nan::AsyncWorker(callback), session(session) {}
		~FixSessionAsyncWorker() {}

		void HandleOKCallback () {
			Nan::HandleScope scope;

            v8::Local<v8::Function> fn = callback->GetFunction();
	        if(!(fn->IsUndefined() || fn->IsNull())) {
				Local<Value> argv[] = {
					Nan::Null()
				};

				callback->Call(1, argv);
			}

		};
	protected:
		FIX::Session* session;
};

class FixSessionDisconnectWorker : public FixSessionAsyncWorker {
	public:
		FixSessionDisconnectWorker(Nan::Callback *callback, FIX::Session* session)
				: FixSessionAsyncWorker(callback, session) {}
		~FixSessionDisconnectWorker() {}

		void Execute () {
			session->disconnect();
		};
};

class FixSessionLogonWorker : public FixSessionAsyncWorker {
	public:
		FixSessionLogonWorker(Nan::Callback *callback, FIX::Session* session)
				: FixSessionAsyncWorker(callback, session) {}
		~FixSessionLogonWorker() {}

		void Execute () {
			session->logon();
		};
};

class FixSessionLogoutWorker : public FixSessionAsyncWorker {
	public:
		FixSessionLogoutWorker(Nan::Callback *callback, FIX::Session* session)
				: FixSessionAsyncWorker(callback, session) {}
		~FixSessionLogoutWorker() {}

		void Execute () {
			session->logout();
		};
};

class FixSessionRefreshWorker : public FixSessionAsyncWorker {
	public:
		FixSessionRefreshWorker(Nan::Callback *callback, FIX::Session* session)
				: FixSessionAsyncWorker(callback, session) {}
		~FixSessionRefreshWorker() {}

		void Execute () {
			session->refresh();
		};
};

class FixSessionResetWorker : public FixSessionAsyncWorker {
	public:
		FixSessionResetWorker(Nan::Callback *callback, FIX::Session* session)
				: FixSessionAsyncWorker(callback, session) {}
		~FixSessionResetWorker() {}

		void Execute () {
			session->reset();
		};
};

FixSession::FixSession() : Nan::ObjectWrap() {
}

FixSession::~FixSession() {
}

void FixSession::setSession(FIX::Session* session) {
	mSession = session;
}

void FixSession::Initialize() {
	Nan::HandleScope scope;

	Local<FunctionTemplate> ctor = Nan::New<FunctionTemplate>(FixSession::New);

	ctor->InstanceTemplate()->SetInternalFieldCount(1);
	ctor->SetClassName(Nan::New("FixSession").ToLocalChecked());

	Local<ObjectTemplate> proto = ctor->PrototypeTemplate();

	Nan::SetPrototypeMethod(ctor, "disconnect", disconnect);
	Nan::SetPrototypeMethod(ctor, "getSessionID", getSessionID);
	Nan::SetPrototypeMethod(ctor, "isEnabled", isEnabled);
	Nan::SetPrototypeMethod(ctor, "isLoggedOn", isLoggedOn);
	Nan::SetPrototypeMethod(ctor, "logon", logon);
	Nan::SetPrototypeMethod(ctor, "logout", logout);
	Nan::SetPrototypeMethod(ctor, "refresh", refresh);
	Nan::SetPrototypeMethod(ctor, "reset", reset);

	Nan::SetAccessor(proto, Nan::New("senderSeqNum").ToLocalChecked(), getSenderSeqNum, setSenderSeqNum);
	Nan::SetAccessor(proto, Nan::New("targetSeqNum").ToLocalChecked(), getTargetSeqNum, setTargetSeqNum);

	g_Ctor.Reset(ctor->GetFunction());
}

Handle<Object> FixSession::wrapFixSession(FIX::Session *session) {
	Nan::EscapableHandleScope scope;
    FixSession* fs = new FixSession();
    fs->setSession(session);

	Local<Function> ctor = Nan::New<Function>(g_Ctor);
	Local<Object> instance = Nan::NewInstance(ctor, 0, {}).ToLocalChecked();

	fs->Wrap(instance);
    return scope.Escape(instance);
}

NAN_METHOD(FixSession::New) {
	Nan::HandleScope scope;

	FixSession *fixSession = new FixSession();

	fixSession->Wrap(info.This());

	info.GetReturnValue().Set(info.This());
}

NAN_METHOD(FixSession::isEnabled) {
	Nan::HandleScope scope;

	FixSession* instance = Nan::ObjectWrap::Unwrap<FixSession>(info.Holder());
	bool isEnabled = instance->mSession->isEnabled();

	info.GetReturnValue().Set(isEnabled ? Nan::True() : Nan::False());
}

NAN_METHOD(FixSession::isLoggedOn) {
	Nan::HandleScope scope;

	FixSession* instance = Nan::ObjectWrap::Unwrap<FixSession>(info.Holder());
	bool isLoggedOn = instance->mSession->isLoggedOn();

	info.GetReturnValue().Set(isLoggedOn ? Nan::True() : Nan::False());
}

NAN_METHOD(FixSession::getSessionID) {
	Nan::HandleScope scope;

	FixSession* instance = Nan::ObjectWrap::Unwrap<FixSession>(info.Holder());
	FIX::SessionID sessionId = instance->mSession->getSessionID();
	Handle<Value> jsSessionId = FixMessageUtil::sessionIdToJs(&sessionId);

	info.GetReturnValue().Set(jsSessionId);
}

NAN_METHOD(FixSession::disconnect) {
	Nan::HandleScope scope;

	FixSession* instance = Nan::ObjectWrap::Unwrap<FixSession>(info.Holder());
	Nan::Callback *callback = new Nan::Callback(info[0].As<Function>());

	Nan::AsyncQueueWorker(new FixSessionDisconnectWorker(callback, instance->mSession));

	return;
}

NAN_METHOD(FixSession::logon) {
	Nan::HandleScope scope;

	FixSession* instance = Nan::ObjectWrap::Unwrap<FixSession>(info.Holder());
	Nan::Callback *callback = new Nan::Callback(info[0].As<Function>());

	Nan::AsyncQueueWorker(new FixSessionLogonWorker(callback, instance->mSession));

	return;
}

NAN_METHOD(FixSession::logout) {
	Nan::HandleScope scope;

	FixSession* instance = Nan::ObjectWrap::Unwrap<FixSession>(info.Holder());
	Nan::Callback *callback = new Nan::Callback(info[0].As<Function>());

	Nan::AsyncQueueWorker(new FixSessionLogoutWorker(callback, instance->mSession));

	return;
}

NAN_METHOD(FixSession::refresh) {
	Nan::HandleScope scope;

	FixSession* instance = Nan::ObjectWrap::Unwrap<FixSession>(info.Holder());
	Nan::Callback *callback = new Nan::Callback(info[0].As<Function>());

	Nan::AsyncQueueWorker(new FixSessionRefreshWorker(callback, instance->mSession));

	return;
}

NAN_METHOD(FixSession::reset) {
	Nan::HandleScope scope;

	FixSession* instance = Nan::ObjectWrap::Unwrap<FixSession>(info.Holder());
	Nan::Callback *callback = new Nan::Callback(info[0].As<Function>());

	Nan::AsyncQueueWorker(new FixSessionResetWorker(callback, instance->mSession));

	return;
}

NAN_GETTER(FixSession::getSenderSeqNum) {
	Nan::HandleScope scope;
	FixSession* instance = Nan::ObjectWrap::Unwrap<FixSession>(info.This());
	info.GetReturnValue().Set(Nan::New<Number>(instance->mSession->getExpectedSenderNum()));
}

NAN_SETTER(FixSession::setSenderSeqNum) {
	Nan::HandleScope scope;
	if(value->IsNumber()) {
		FixSession* instance = Nan::ObjectWrap::Unwrap<FixSession>(info.This());
		instance->mSession->setNextSenderMsgSeqNum(value->Uint32Value());
	}
}

NAN_GETTER(FixSession::getTargetSeqNum) {
	Nan::HandleScope scope;
	FixSession* instance = Nan::ObjectWrap::Unwrap<FixSession>(info.This());
	info.GetReturnValue().Set(Nan::New<Number>(instance->mSession->getExpectedTargetNum()));
}

NAN_SETTER(FixSession::setTargetSeqNum) {
	Nan::HandleScope scope;
	if(value->IsNumber()) {
		FixSession* instance = Nan::ObjectWrap::Unwrap<FixSession>(info.This());
		instance->mSession->setNextTargetMsgSeqNum(value->Uint32Value());
	}
}



