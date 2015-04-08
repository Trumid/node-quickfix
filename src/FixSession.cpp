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

			if(!callback->IsEmpty()) {
				Local<Value> argv[] = {
					NanNull()
				};

				callback->Call(1, argv);
			}

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

class FixSessionRefreshWorker : public FixSessionAsyncWorker {
	public:
		FixSessionRefreshWorker(NanCallback *callback, FIX::Session* session)
				: FixSessionAsyncWorker(callback, session) {}
		~FixSessionRefreshWorker() {}

		void Execute () {
			session->refresh();
		};
};

class FixSessionResetWorker : public FixSessionAsyncWorker {
	public:
		FixSessionResetWorker(NanCallback *callback, FIX::Session* session)
				: FixSessionAsyncWorker(callback, session) {}
		~FixSessionResetWorker() {}

		void Execute () {
			session->reset();
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

	ctor->InstanceTemplate()->SetInternalFieldCount(1);
	ctor->SetClassName(NanNew("FixSession"));

	Local<ObjectTemplate> proto = ctor->PrototypeTemplate();

	NODE_SET_PROTOTYPE_METHOD(ctor, "disconnect", disconnect);
	NODE_SET_PROTOTYPE_METHOD(ctor, "getSessionID", getSessionID);
	NODE_SET_PROTOTYPE_METHOD(ctor, "isEnabled", isEnabled);
	NODE_SET_PROTOTYPE_METHOD(ctor, "isLoggedOn", isLoggedOn);
	NODE_SET_PROTOTYPE_METHOD(ctor, "logon", logon);
	NODE_SET_PROTOTYPE_METHOD(ctor, "logout", logout);
	NODE_SET_PROTOTYPE_METHOD(ctor, "refresh", refresh);
	NODE_SET_PROTOTYPE_METHOD(ctor, "reset", reset);

	proto->SetAccessor(NanNew("senderSeqNum"), getSenderSeqNum, setSenderSeqNum);
	proto->SetAccessor(NanNew("targetSeqNum"), getTargetSeqNum, setTargetSeqNum);

	target->Set(NanNew("FixSession"), ctor->GetFunction());
}

Handle<Object> FixSession::wrapFixSession(FixSession* fixSession) {
	Local<FunctionTemplate> ctor = NanNew<FunctionTemplate>();

	ctor->InstanceTemplate()->SetInternalFieldCount(1);
	ctor->SetClassName(NanNew("FixSession"));

	Local<ObjectTemplate> proto = ctor->PrototypeTemplate();

	NODE_SET_PROTOTYPE_METHOD(ctor, "disconnect", disconnect);
	NODE_SET_PROTOTYPE_METHOD(ctor, "getSessionID", getSessionID);
	NODE_SET_PROTOTYPE_METHOD(ctor, "isEnabled", isEnabled);
	NODE_SET_PROTOTYPE_METHOD(ctor, "isLoggedOn", isLoggedOn);
	NODE_SET_PROTOTYPE_METHOD(ctor, "logon", logon);
	NODE_SET_PROTOTYPE_METHOD(ctor, "logout", logout);
	NODE_SET_PROTOTYPE_METHOD(ctor, "refresh", refresh);
	NODE_SET_PROTOTYPE_METHOD(ctor, "reset", reset);

	proto->SetAccessor(NanNew("senderSeqNum"), getSenderSeqNum, setSenderSeqNum);
	proto->SetAccessor(NanNew("targetSeqNum"), getTargetSeqNum, setTargetSeqNum);

	Handle<Object> obj = ctor->InstanceTemplate()->NewInstance();

	//obj->SetAlignedPointerInInternalField(0, NanNew<External>(fixSession));
	fixSession->Wrap(obj);
	return obj;
}

NAN_METHOD(FixSession::New) {
	NanScope();

	FixSession *fixSession = new FixSession();

	fixSession->Wrap(args.This());

	NanReturnValue(args.This());
}

NAN_METHOD(FixSession::isEnabled) {
	NanScope();

	FixSession* instance = ObjectWrap::Unwrap<FixSession>(args.Holder());
	bool isEnabled = instance->mSession->isEnabled();

	NanReturnValue(isEnabled ? NanTrue() : NanFalse());
}

NAN_METHOD(FixSession::isLoggedOn) {
	NanScope();

	FixSession* instance = ObjectWrap::Unwrap<FixSession>(args.Holder());
	bool isLoggedOn = instance->mSession->isLoggedOn();

	NanReturnValue(isLoggedOn ? NanTrue() : NanFalse());
}

NAN_METHOD(FixSession::getSessionID) {
	NanScope();

	FixSession* instance = ObjectWrap::Unwrap<FixSession>(args.Holder());
	FIX::SessionID sessionId = instance->mSession->getSessionID();
	Handle<Value> jsSessionId = FixMessageUtil::sessionIdToJs(&sessionId);

	NanReturnValue(jsSessionId);
}

NAN_METHOD(FixSession::disconnect) {
	NanScope();

	FixSession* instance = ObjectWrap::Unwrap<FixSession>(args.Holder());
	NanCallback *callback = new NanCallback(args[0].As<Function>());

	NanAsyncQueueWorker(new FixSessionDisconnectWorker(callback, instance->mSession));

	NanReturnUndefined();
}

NAN_METHOD(FixSession::logon) {
	NanScope();

	FixSession* instance = ObjectWrap::Unwrap<FixSession>(args.Holder());
	NanCallback *callback = new NanCallback(args[0].As<Function>());

	NanAsyncQueueWorker(new FixSessionLogonWorker(callback, instance->mSession));

	NanReturnUndefined();
}

NAN_METHOD(FixSession::logout) {
	NanScope();

	FixSession* instance = ObjectWrap::Unwrap<FixSession>(args.Holder());
	NanCallback *callback = new NanCallback(args[0].As<Function>());

	NanAsyncQueueWorker(new FixSessionLogoutWorker(callback, instance->mSession));

	NanReturnUndefined();
}

NAN_METHOD(FixSession::refresh) {
	NanScope();

	FixSession* instance = ObjectWrap::Unwrap<FixSession>(args.Holder());
	NanCallback *callback = new NanCallback(args[0].As<Function>());

	NanAsyncQueueWorker(new FixSessionRefreshWorker(callback, instance->mSession));

	NanReturnUndefined();
}

NAN_METHOD(FixSession::reset) {
	NanScope();

	FixSession* instance = ObjectWrap::Unwrap<FixSession>(args.Holder());
	NanCallback *callback = new NanCallback(args[0].As<Function>());

	NanAsyncQueueWorker(new FixSessionResetWorker(callback, instance->mSession));

	NanReturnUndefined();
}

NAN_GETTER(FixSession::getSenderSeqNum) {
	NanScope();
	FixSession* instance = ObjectWrap::Unwrap<FixSession>(args.Holder());
	NanReturnValue(NanNew<Number>(instance->mSession->getExpectedSenderNum()));
}

NAN_SETTER(FixSession::setSenderSeqNum) {
	NanScope();
	if(value->IsNumber()) {
		FixSession* instance = ObjectWrap::Unwrap<FixSession>(args.Holder());
		instance->mSession->setNextSenderMsgSeqNum(value->Uint32Value());
	}
}

NAN_GETTER(FixSession::getTargetSeqNum) {
	NanScope();
	FixSession* instance = ObjectWrap::Unwrap<FixSession>(args.Holder());
	NanReturnValue(NanNew<Number>(instance->mSession->getExpectedTargetNum()));
}

NAN_SETTER(FixSession::setTargetSeqNum) {
	NanScope();
	if(value->IsNumber()) {
		FixSession* instance = ObjectWrap::Unwrap<FixSession>(args.Holder());
		instance->mSession->setNextTargetMsgSeqNum(value->Uint32Value());
	}
}



