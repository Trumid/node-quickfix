#include <v8.h>
#include <node.h>
#include "quickfix/SessionID.h"
#include "quickfix/Message.h"
#include "Dispatcher.h"
#include "FixApplication.h"
#include "FixEvent.h"
#include "FixLoginProvider.h"
#include "FixLoginResponse.h"

using namespace v8;

FixApplication::FixApplication() {}

FixApplication::FixApplication(v8::Persistent<v8::Object>* callbacks) 
	: mCallbacks(callbacks)
{
}

FixApplication::~FixApplication()
{
}

void FixApplication::onCreate( const FIX::SessionID& sessionID )
{
	// std::cout << "onCreate " << sessionID.toString() << '\n';

	fix_event_t *data = new fix_event_t;
	data->eventName = std::string("onCreate");
	data->callbacks = mCallbacks;
	data->sessionId = &sessionID;

	Dispatcher::getInstance().dispatch(data);
}

void FixApplication::onLogon( const FIX::SessionID& sessionID )
{
	// std::cout << "onLogon " << sessionID.toString() << '\n';

	fix_event_t *data = new fix_event_t;
	data->eventName = std::string("onLogon");
	data->callbacks = mCallbacks;
	data->sessionId = &sessionID;

	Dispatcher::getInstance().dispatch(data);
}

void FixApplication::onLogout( const FIX::SessionID& sessionID )
{
	// std::cout << "onLogout " << sessionID.toString() << '\n';

	fix_event_t *data = new fix_event_t;
	data->eventName = std::string("onLogout");
	data->callbacks = mCallbacks;
	data->sessionId = &sessionID;

	Dispatcher::getInstance().dispatch(data);
}

void FixApplication::toAdmin( FIX::Message& message, const FIX::SessionID& sessionID )
{
	// std::cout << "toAdmin " << sessionID.toString() << '\n';

	if(strcmp(message.getHeader().getField(35).c_str(), "A") == 0 && mCredentials != NULL) {
		message.setField(553, mCredentials->username.c_str());
		message.setField(554, mCredentials->password.c_str());
	}

	fix_event_t *data = new fix_event_t;
	data->eventName = std::string("toAdmin");
	data->callbacks = mCallbacks;
	data->sessionId = &sessionID;
	data->message = new FIX::Message(message);

	Dispatcher::getInstance().dispatch(data);
}

void FixApplication::fromAdmin( const FIX::Message& message, const FIX::SessionID& sessionID )
	throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon)
{
	// std::cout << "fromAdmin " << sessionID.toString() << '\n';

  fix_event_t *data = new fix_event_t;
  data->eventName = std::string("fromAdmin");
  data->message = new FIX::Message(message);
  data->sessionId = &sessionID;
  data->callbacks = mCallbacks;
  Dispatcher::getInstance().dispatch(data);

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
		  delete logonResponse;
		  throw FIX::RejectLogon();
	  }

	  delete logonResponse;
  }

}

void FixApplication::toApp( FIX::Message& message, const FIX::SessionID& sessionID )
throw( FIX::DoNotSend )
{
	// std::cout << "toApp " << sessionID.toString() << '\n';

	fix_event_t *data = new fix_event_t;
	data->eventName = std::string("toApp");
	data->callbacks = mCallbacks;
	data->sessionId = &sessionID;
	data->message = new FIX::Message(message);
	
	Dispatcher::getInstance().dispatch(data);
}

void FixApplication::fromApp( const FIX::Message& message, const FIX::SessionID& sessionID )
	throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType )
{
	// std::cout << "fromApp " << sessionID.toString() << '\n';

	fix_event_t *data = new fix_event_t;
	data->eventName = std::string("fromApp");
	data->callbacks = mCallbacks;
	data->sessionId = &sessionID;
	data->message = new FIX::Message(message);
	
	Dispatcher::getInstance().dispatch(data);
}

void FixApplication::setLogonProvider(FixLoginProvider* loginProvider) {
	mLoginProvider = loginProvider;
}

void FixApplication::setCredentials(fix_credentials* credentials) {
	mCredentials = credentials;
}

