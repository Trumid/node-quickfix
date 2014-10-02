/*
 * FixLoginProvider.cpp
 *
 *  Created on: Sep 30, 2014
 *      Author: kdeol
 */
#include <iostream>
#include "FixLoginProvider.h"
using namespace v8;
using namespace node;
using namespace std;

FixLoginProvider::FixLoginProvider() : ObjectWrap() {

}

FixLoginProvider::~FixLoginProvider() {
}

void FixLoginProvider::Initialize(Handle<Object> target) {
  NanScope();

  Local<FunctionTemplate> ctor = NanNew<FunctionTemplate>(FixLoginProvider::New);

  // TODO:: Figure out what the compile error is with this
  //NanAssignPersistent(constructor, ctor);

  ctor->InstanceTemplate()->SetInternalFieldCount(1);
  ctor->SetClassName(NanNew("FixLoginProvider"));

  NODE_SET_PROTOTYPE_METHOD(ctor, "finish", finish);

  target->Set(NanNew("FixLoginProvider"), ctor->GetFunction());
}

NAN_METHOD(FixLoginProvider::New) {
	NanScope();

	FixLoginProvider *loginProvider = new FixLoginProvider();

	loginProvider->Wrap(args.This());
	loginProvider->logon = new NanCallback(args[0].As<Function>());

	NanReturnValue(args.This());
}

NAN_METHOD(FixLoginProvider::finish) {
	NanScope();

	FixLoginProvider* instance = ObjectWrap::Unwrap<FixLoginProvider>(args.This());

	bool success = args[0]->ToBoolean()->BooleanValue();

	instance->setIsFinished(true);
	instance->setIsLoggedOn(success);

	NanReturnUndefined();
}

bool FixLoginProvider::getIsFinished() {
	return isFinished;
}

bool FixLoginProvider::getIsLoggedOn() {
	return isLoggedOn;
}

void FixLoginProvider::setIsFinished(bool finished) {
	isFinished = finished;
}

void FixLoginProvider::setIsLoggedOn(bool loggedOn) {
	isLoggedOn = loggedOn;
}

NanCallback* FixLoginProvider::getLogon() {
	return logon;
}
