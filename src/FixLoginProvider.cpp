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
  // NanAssignPersistent(constructor, ctor);

  ctor->InstanceTemplate()->SetInternalFieldCount(1);
  ctor->SetClassName(NanNew("FixLoginProvider"));

  target->Set(NanNew("FixLoginProvider"), ctor->GetFunction());
}

NAN_METHOD(FixLoginProvider::New) {
	NanScope();

	FixLoginProvider *loginProvider = new FixLoginProvider();

	loginProvider->Wrap(args.This());
	loginProvider->logon = new NanCallback(args[0].As<Function>());

	NanReturnValue(args.This());
}

NanCallback* FixLoginProvider::getLogon() {
	return logon;
}
