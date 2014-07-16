/*
 * FixEventHandler.cpp
 *
 *  Created on: Jul 14, 2014
 *      Author: kdeol
 */
#include <node.h>
#include "FixEventHandler.h"
#include "FixEvent.h"
#include "quickfix/Message.h"
#include "quickfix/SessionID.h"
#include "quickfix/FieldMap.h"
#include "quickfix/Field.h"
#include "quickfix/MessageSorters.h"

using namespace v8;
using namespace std;
FixEventHandler::FixEventHandler(){}

FixEventHandler::FixEventHandler(const Arguments& args) {
	HandleScope scope;

	mApp = Persistent<Object>::New(args[0]->IsUndefined() ? Object::New() : args[0]->ToObject());

	scope.Close(Undefined());
}

FixEventHandler::~FixEventHandler() {
	mApp.Dispose();
}

static Handle<Value> sessionIdToJs(FIX::SessionID* sessionId) {
	Handle<Object> session = Object::New();
	session->Set(String::New("beginString"), String::New(sessionId->getBeginString().getString().c_str()));
	session->Set(String::New("senderCompID"), String::New(sessionId->getSenderCompID().getString().c_str()));
	session->Set(String::New("targetCompID"), String::New(sessionId->getTargetCompID().getString().c_str()));
	session->Set(String::New("sessionQualifier"), String::New(sessionId->getSessionQualifier().c_str()));

	return session;
}

static Handle<Value> messageToJs(FIX::Message* message) {
	Handle<Object> msg = Object::New();
	FIX::Header messageHeader = message->getHeader();
	Handle<Array> header = Array::New(messageHeader.totalFields());
	Handle<Array> tags = Array::New(message->totalFields()); //Do I need to subtract header count here?

	int i=0;
	for(FIX::FieldMap::iterator it = messageHeader.begin(); it != messageHeader.end(); ++it)
	{
		Handle<Object> field = Object::New();
		field->Set(String::New("id"), Integer::New(it->first));
		field->Set(String::New("value"), String::New(it->second.getString().c_str()));
		header->Set(i, field);
		++i;
	}

	int j=0;
	for(FIX::FieldMap::iterator it = message->begin(); it != message->end(); ++it)
	{
		Handle<Object> field = Object::New();
		field->Set(String::New("id"), Integer::New(it->first));
		field->Set(String::New("value"), String::New(it->second.getString().c_str()));
		tags->Set(j, field);
		++j;
	}

	msg->Set(String::New("header"), header);
	msg->Set(String::New("tags"), tags);
	return msg;
}

void FixEventHandler::handleFixEvent(uv_async_t *handle, int status) {
	HandleScope scope;
	FixEvent event = *((FixEvent*)handle->data);
	FixEventHandler* handler = event.handler;

	Local<String> name = String::New(event.eventName.c_str());
	Persistent<Object> app = handler->mApp;
	//Also make sure it is a function
	if(app->Has(name))
	{
		Local<Function> callback = Local<Function>::Cast(app->Get(name));

		if(event.message != NULL) {
			const unsigned argc = 2;
			Handle<Value> argv[argc] = { messageToJs(event.message), sessionIdToJs(event.sessionId) };
			callback->Call(Context::GetCurrent()->Global(), argc, argv);
		} else {
			const unsigned argc = 1;
			Handle<Value> argv[argc] = { sessionIdToJs(event.sessionId) };
			callback->Call(Context::GetCurrent()->Global(), argc, argv);
		}

	}
}

Persistent<Object> FixEventHandler::getApp() {
	return mApp;
}



