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

class FixLoginProvider : public Nan::ObjectWrap {
	public:
		static NAN_MODULE_INIT(Init);
		
		static NAN_METHOD(New);
		Nan::Callback* getLogon();

	private:
		~FixLoginProvider();

		static Nan::Persistent<v8::Function> constructor;
		Nan::Callback* logon;
};

#endif /* FIXLOGINPROVIDER_H_ */
