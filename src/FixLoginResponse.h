/*
 * FixLoginResponse.h
 *
 *  Created on: Mar 29, 2015
 *      Author: kdeol
 */

#ifndef FIXLOGINRESPONSE_H_
#define FIXLOGINRESPONSE_H_

#include <nan.h>

using namespace v8;
using namespace node;

class FixLoginResponse : public Nan::ObjectWrap {
public:
	FixLoginResponse();
	virtual ~FixLoginResponse();
	static void Initialize(Handle<Object> target);
	static NAN_METHOD(New);
	bool getIsFinished();
	bool getIsLoggedOn();
	void setIsFinished(bool isFinished);
	void setIsLoggedOn(bool isLoggedOn);
	static void close(FixLoginResponse* fixLoginResponse);
	static Handle<Object> wrapFixLoginResponse(FixLoginResponse* fixLoginResponse);

private:
	bool isFinished = false;
	bool isLoggedOn = false;
	static NAN_METHOD(done);
};

#endif /* FIXLOGINRESPONSE_H_ */
