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
	bool getIsFinished();
	bool getIsLoggedOn();
	NanCallback* getLogon();
	void setIsFinished(bool isFinished);
	void setIsLoggedOn(bool isLoggedOn);
	static void Initialize(v8::Handle<v8::Object> target);
	static NAN_METHOD(New);

private:
	virtual ~FixLoginProvider();
	bool isFinished = false;
	bool isLoggedOn = false;
	NanCallback* logon;
	static NAN_METHOD(finish);
};

#endif /* FIXLOGINPROVIDER_H_ */
