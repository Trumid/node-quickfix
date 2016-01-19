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

FixLoginProvider::FixLoginProvider() : Nan::ObjectWrap() {

}

FixLoginProvider::~FixLoginProvider() {
}

void FixLoginProvider::Initialize(Handle<Object> target) {
  Nan::HandleScope scope;

  Local<FunctionTemplate> ctor = Nan::New<FunctionTemplate>(FixLoginProvider::New);

  // TODO:: Figure out what the compile error is with this
  // constructor.Reset(ctor);

  ctor->InstanceTemplate()->SetInternalFieldCount(1);
  ctor->SetClassName(Nan::New("FixLoginProvider").ToLocalChecked());

  target->Set(Nan::New("FixLoginProvider").ToLocalChecked(), ctor->GetFunction());
}

NAN_METHOD(FixLoginProvider::New) {
	Nan::HandleScope scope;

	FixLoginProvider *loginProvider = new FixLoginProvider();

	loginProvider->Wrap(info.This());
	loginProvider->logon = new Nan::Callback(info[0].As<Function>());

	info.GetReturnValue().Set(info.This());
}

Nan::Callback* FixLoginProvider::getLogon() {
	return logon;
}
