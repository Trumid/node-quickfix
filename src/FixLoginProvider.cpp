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

Nan::Persistent<Function> FixLoginProvider::constructor;

NAN_MODULE_INIT(FixLoginProvider::Init) {
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);

  tpl->SetClassName(Nan::New("FixLoginProvider").ToLocalChecked());
	tpl->InstanceTemplate()->SetInternalFieldCount(1);

	constructor.Reset(Nan::GetFunction(tpl).ToLocalChecked());
  Nan::Set(target, Nan::New("FixLoginProvider").ToLocalChecked(), Nan::GetFunction(tpl).ToLocalChecked());
}

NAN_METHOD(FixLoginProvider::New) {
	FixLoginProvider *loginProvider = new FixLoginProvider();

	loginProvider->Wrap(info.This());
	loginProvider->logon = new Nan::Callback(info[0].As<Function>());

	info.GetReturnValue().Set(info.This());
}

Nan::Callback* FixLoginProvider::getLogon() {
	return logon;
}

FixLoginProvider::~FixLoginProvider() {
}
