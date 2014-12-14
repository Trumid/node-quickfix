#include <v8.h>
#include <node.h>
#include <chrono>
#include <thread>
#include "quickfix/SessionID.h"
#include "quickfix/Message.h"
#include "FixApplication.h"
#include "FixEvent.h"
#include "FixEventQueue.h"
#include "FixLoginProvider.h"

FixApplication::FixApplication() {}

FixApplication::FixApplication(
		uv_async_t* handle,
		uv_async_t* logonHandle,
		v8::Persistent<v8::Object>* callbacks) :
		mAsyncHandle(handle), mLogonHandle(logonHandle), mCallbacks(callbacks)
{
}

FixApplication::~FixApplication()
{
}

void FixApplication::onLogon( const FIX::SessionID& sessionID )
{
	fix_event_t *data = new fix_event_t;
	data->eventName = std::string("onLogon");
	data->sessionId = &sessionID;
	data->callbacks = mCallbacks;
	data->message = NULL;

	fix_event_queue_t* queueHandle = new fix_event_queue_t;
	queueHandle->queue = &eventQueue;
	mAsyncHandle->data = queueHandle;

	eventQueue.push(data);
	uv_async_send(mAsyncHandle);
}

void FixApplication::onLogout( const FIX::SessionID& sessionID )
{
	fix_event_t *data = new fix_event_t;
	data->eventName = std::string("onLogout");
	data->sessionId = &sessionID;
	data->callbacks = mCallbacks;
	data->message = NULL;

	fix_event_queue_t* queueHandle = new fix_event_queue_t;
	queueHandle->queue = &eventQueue;
	mAsyncHandle->data = queueHandle;

	eventQueue.push(data);
	uv_async_send(mAsyncHandle);
}

void FixApplication::fromApp( const FIX::Message& message, const FIX::SessionID& sessionID )
throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType )
{
	fix_event_t *data = new fix_event_t;
	data->eventName = std::string("fromApp");
	data->sessionId = &sessionID;
	data->message = new FIX::Message(message);
	data->callbacks = mCallbacks;

	fix_event_queue_t* queueHandle = new fix_event_queue_t;
	queueHandle->queue = &eventQueue;
	mAsyncHandle->data = queueHandle;

	eventQueue.push(data);
	uv_async_send(mAsyncHandle);
}

void FixApplication::toApp( FIX::Message& message, const FIX::SessionID& sessionID )
throw( FIX::DoNotSend )
{
	fix_event_t *data = new fix_event_t;
	data->eventName = std::string("toApp");
	data->sessionId = &sessionID;
	data->message = new FIX::Message(message);
	data->callbacks = mCallbacks;

	fix_event_queue_t* queueHandle = new fix_event_queue_t;
	queueHandle->queue = &eventQueue;
	mAsyncHandle->data = queueHandle;
	
	eventQueue.push(data);
	uv_async_send(mAsyncHandle);
}

void FixApplication::setLogonProvider(FixLoginProvider* loginProvider) {
	mLoginProvider = loginProvider;
}

void FixApplication::setCredentials(fix_credentials* credentials) {
	mCredentials = credentials;
}

