#include <v8.h>
#include <node.h>
#include "quickfix/SessionID.h"
#include "quickfix/Message.h"
#include "FixApplication.h"
#include "FixEvent.h"

FixApplication::FixApplication() {}

FixApplication::FixApplication(uv_async_t* handle, v8::Persistent<v8::Object>* callbacks) :
		mAsyncHandle(handle), mCallbacks(callbacks)
{
}

FixApplication::~FixApplication()
{
}

void FixApplication::onLogon( const FIX::SessionID& sessionID )
{
	std::cout << "FIX onLogon" << std::endl;

	fix_event_t *data = new fix_event_t;
	data->eventName = std::string("onLogon");
	data->sessionId = &sessionID;
	data->callbacks = mCallbacks;
	data->message = NULL;
	mAsyncHandle->data = data;

	uv_async_send(mAsyncHandle);
}

void FixApplication::onLogout( const FIX::SessionID& sessionID )
{
	std::cout << "FIX onLogout" << std::endl;

	fix_event_t *data = new fix_event_t;
	data->eventName = std::string("onLogout");
	data->sessionId = &sessionID;
	data->callbacks = mCallbacks;
	data->message = NULL;
	mAsyncHandle->data = data;

	uv_async_send(mAsyncHandle);
}

void FixApplication::fromApp( const FIX::Message& message, const FIX::SessionID& sessionID )
throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType )
{
	std::cout << "FIX fromApp" << std::endl;

	fix_event_t *data = new fix_event_t;
	data->eventName = std::string("fromApp");
	data->sessionId = &sessionID;
	data->message = new FIX::Message(message);
	data->callbacks = mCallbacks;
	mAsyncHandle->data = data;

	uv_async_send(mAsyncHandle);
}

void FixApplication::toApp( FIX::Message& message, const FIX::SessionID& sessionID )
throw( FIX::DoNotSend )
{
	std::cout << "FIX toApp" << std::endl;

	fix_event_t *data = new fix_event_t;
	data->eventName = std::string("toApp");
	data->sessionId = &sessionID;
	data->message = new FIX::Message(message);
	data->callbacks = mCallbacks;
	mAsyncHandle->data = data;

	uv_async_send(mAsyncHandle);
}

