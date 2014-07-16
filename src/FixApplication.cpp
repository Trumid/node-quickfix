#include <node.h>
#include "quickfix/SessionID.h"
#include "quickfix/Message.h"
#include "FixApplication.h"
#include "FixEventHandler.h"
#include "FixEvent.h"

FixApplication::FixApplication() {}

FixApplication::FixApplication(FixEventHandler* handler, uv_async_t* handle) :
		mEventHandler(handler), mAsyncHandle(handle)
{
}

FixApplication::~FixApplication()
{
}

void FixApplication::onLogon( const FIX::SessionID& sessionID )
{
	std::cout <<"Logged on" << std::endl;
	FixEvent* event = new FixEvent();

	event->eventName = "onLogon";
	event->handler = mEventHandler;
	event->sessionId = new FIX::SessionID(sessionID);

	mAsyncHandle->data = (void*) event;

	uv_async_send(mAsyncHandle);
}

void FixApplication::onLogout( const FIX::SessionID& sessionID )
{
	std::cout <<"Logged out" << std::endl;
	FixEvent* event = new FixEvent();

	event->eventName = "onLogout";
	event->handler = mEventHandler;
	event->sessionId = new FIX::SessionID(sessionID);

	mAsyncHandle->data = (void*) event;

	uv_async_send(mAsyncHandle);
}

void FixApplication::fromApp( const FIX::Message& message, const FIX::SessionID& sessionID )
throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType )
{
	std::cout <<"Message received in fromApp:" << std::endl;
	std::cout << message.toString() << std::endl;

	FixEvent* event = new FixEvent();
	event->eventName = "fromApp";
	event->handler = mEventHandler;
	event->message = new FIX::Message(message);
	event->sessionId = new FIX::SessionID(sessionID);

	mAsyncHandle->data = (void*) event;

	std::cout << "Alerting main thread from fromApp event" << std::endl;
	uv_async_send(mAsyncHandle);
}

void FixApplication::toApp( FIX::Message& message, const FIX::SessionID& sessionID )
throw( FIX::DoNotSend )
{
	std::cout <<"Message sent in toApp:" << std::endl;
	std::cout << message.toString() << std::endl;

	FixEvent* event = new FixEvent();
	event->eventName = "toApp";
	event->handler = mEventHandler;
	event->message = new FIX::Message(message);
	event->sessionId = new FIX::SessionID(sessionID);

	mAsyncHandle->data = (void*) event;

	uv_async_send(mAsyncHandle);
}
