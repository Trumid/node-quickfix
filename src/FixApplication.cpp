#include <v8.h>
#include <node.h>
#include <nan.h>
#include "quickfix/SessionID.h"
#include "quickfix/Message.h"
#include "Dispatcher.h"
#include "FixApplication.h"
#include "FixEvent.h"
#include "FixLoginProvider.h"
#include "FixLoginResponse.h"
#include <unordered_set>

using namespace v8;

FixApplication::FixApplication() {}

FixApplication::FixApplication(Nan::Persistent<v8::Object>* callbacks, std::unordered_set<std::string>* callbackRegistry)
{
	mCallbacks = callbacks;
	mCallbackRegistry = callbackRegistry;
}

FixApplication::~FixApplication()
{
}

void FixApplication::onCreate( const FIX::SessionID& sessionID )
{
	FixApplication::dispatchEvent(std::string("onCreate"), sessionID);
}

void FixApplication::onLogon( const FIX::SessionID& sessionID )
{
	FixApplication::dispatchEvent(std::string("onLogon"), sessionID);
}

void FixApplication::onLogout( const FIX::SessionID& sessionID )
{
	FixApplication::dispatchEvent(std::string("onLogout"), sessionID);
}

void FixApplication::toAdmin( FIX::Message& message, const FIX::SessionID& sessionID )
{
	if(strcmp(message.getHeader().getField(35).c_str(), "A") == 0 && mCredentials != NULL) {
		message.setField(553, mCredentials->username.c_str());
		message.setField(554, mCredentials->password.c_str());
	}

	FixApplication::dispatchEvent(std::string("toAdmin"), message, sessionID);
}

void FixApplication::fromAdmin( const FIX::Message& message, const FIX::SessionID& sessionID )
	throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon)
{
	FixApplication::dispatchEvent(std::string("fromAdmin"), message, sessionID);

	if(strcmp(message.getHeader().getField(35).c_str(), "A") == 0 && mLoginProvider != NULL) {
	  fix_event_t *data = new fix_event_t;
	  data->eventName = std::string("onLogonAttempt");
	  data->callbacks = mCallbacks;
	  data->sessionId = &sessionID;
	  data->message = new FIX::Message(message);
	  data->logon = mLoginProvider->getLogon();

	  FixLoginResponse* logonResponse = new FixLoginResponse();
	  data->logonResponse = logonResponse;

	  Dispatcher::getInstance().dispatch(data);

	  while(!logonResponse->getIsFinished()) {
	  }

	  if(!logonResponse->getIsLoggedOn()) {
		  //delete logonResponse;
		  throw FIX::RejectLogon();
	  }

	  //delete logonResponse;
  }

}

void FixApplication::toApp( FIX::Message& message, const FIX::SessionID& sessionID )
throw( FIX::DoNotSend )
{
	FixApplication::dispatchEvent(std::string("toApp"), message, sessionID);
}

void FixApplication::fromApp( const FIX::Message& message, const FIX::SessionID& sessionID )
	throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType )
{
	FixApplication::dispatchEvent(std::string("fromApp"), message, sessionID);
}

void FixApplication::dispatchEvent(std::string eventName, const FIX::Message& message, const FIX::SessionID& sessionID) {
	std::unordered_set<std::string>::const_iterator got = mCallbackRegistry->find(eventName);

	if ( got == mCallbackRegistry->end() ) {
		return;
	} else {
		fix_event_t *data = new fix_event_t;
		data->eventName = eventName;
		data->callbacks = mCallbacks;
		data->sessionId = &sessionID;
		data->message = new FIX::Message(message);

		Dispatcher::getInstance().dispatch(data);
	}
}

void FixApplication::dispatchEvent(std::string eventName, const FIX::SessionID& sessionID) {
	std::unordered_set<std::string>::const_iterator got = mCallbackRegistry->find(eventName);

	if ( got == mCallbackRegistry->end() ) {
		return;
	} else {
		fix_event_t *data = new fix_event_t;
		data->eventName = eventName;
		data->callbacks = mCallbacks;
		data->sessionId = &sessionID;

		Dispatcher::getInstance().dispatch(data);
	}
}

void FixApplication::setLogonProvider(FixLoginProvider* loginProvider) {
	mLoginProvider = loginProvider;
}

void FixApplication::setCredentials(fix_credentials* credentials) {
	mCredentials = credentials;
}

void FixApplication::setCallbacks(Nan::Persistent<v8::Object>* callbacks) {
	mCallbacks = callbacks;
}

void FixApplication::setCallbackRegistry(std::unordered_set<std::string>* callbackRegistry) {
	mCallbackRegistry = callbackRegistry;
}
