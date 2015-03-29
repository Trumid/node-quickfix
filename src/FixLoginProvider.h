/*
 * FixLoginProvider.h
 *
 *  Created on: Sep 30, 2014
 *      Author: kdeol
 */

#ifndef FIXLOGINPROVIDER_H_
#define FIXLOGINPROVIDER_H_

#include <v8.h>
#include <node.h>
#include <nan.h>

class FixLoginProvider : public node::ObjectWrap {
public:
	FixLoginProvider();
	static void Initialize(v8::Handle<v8::Object> target);
	static NAN_METHOD(New);
	NanCallback* getLogon();

private:
	virtual ~FixLoginProvider();
	NanCallback* logon;
};

#endif /* FIXLOGINPROVIDER_H_ */
