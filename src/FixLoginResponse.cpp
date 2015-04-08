/*
 * FixLoginResponse.cpp
 *
 *  Created on: Mar 29, 2015
 *      Author: kdeol
 */

#include "FixLoginResponse.h"
#include <iostream>
using namespace v8;
using namespace node;
using namespace std;

FixLoginResponse::FixLoginResponse() : ObjectWrap(){

}

FixLoginResponse::~FixLoginResponse() {
}

void FixLoginResponse::Initialize(Handle<Object> target) {
  NanScope();

  Local<FunctionTemplate> ctor = NanNew<FunctionTemplate>(FixLoginResponse::New);

  ctor->InstanceTemplate()->SetInternalFieldCount(1);
  ctor->SetClassName(NanNew("FixLoginResponse"));

  NODE_SET_PROTOTYPE_METHOD(ctor, "done", done);

  target->Set(NanNew("FixLoginResponse"), ctor->GetFunction());
}

Handle<Object> FixLoginResponse::wrapFixLoginResponse(FixLoginResponse* fixLoginResponse) {

	Local<FunctionTemplate> ctor = NanNew<FunctionTemplate>();

	ctor->InstanceTemplate()->SetInternalFieldCount(1);
	ctor->SetClassName(NanNew("FixLoginResponse"));

	Local<ObjectTemplate> proto = ctor->PrototypeTemplate();

	NODE_SET_PROTOTYPE_METHOD(ctor, "done", done);

	Handle<Object> obj = ctor->InstanceTemplate()->NewInstance();
	//obj->SetInternalField(0, NanNew<External>(fixLoginResponse));

	fixLoginResponse->Wrap(obj);
	fixLoginResponse->Ref();

	return obj;
}

NAN_METHOD(FixLoginResponse::New) {
	NanScope();

	FixLoginResponse *loginResponse = new FixLoginResponse();

	loginResponse->Wrap(args.This());

	NanReturnValue(args.This());
}

NAN_METHOD(FixLoginResponse::done) {
	NanScope();

	FixLoginResponse* instance = ObjectWrap::Unwrap<FixLoginResponse>(args.This());

	bool success = args[0]->ToBoolean()->BooleanValue();

	instance->setIsFinished(true);
	instance->setIsLoggedOn(success);

	NanReturnUndefined();
}

bool FixLoginResponse::getIsFinished() {
	return isFinished;
}

bool FixLoginResponse::getIsLoggedOn() {
	return isLoggedOn;
}

void FixLoginResponse::setIsFinished(bool finished) {
	isFinished = finished;
}

void FixLoginResponse::setIsLoggedOn(bool loggedOn) {
	isLoggedOn = loggedOn;
}

void FixLoginResponse::close(FixLoginResponse* fixLoginResponse) {
	fixLoginResponse->Unref();
}

