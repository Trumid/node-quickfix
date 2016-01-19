#ifndef APPLICATION_H
#define APPLICATION_H

#include <v8.h>
#include <node.h>
#include <nan.h>

#include "quickfix/Application.h"
#include "quickfix/SessionID.h"
#include "quickfix/Message.h"
#include "FixLoginProvider.h"
#include "FixCredentials.h"
#include "FixEvent.h"
#include <unordered_set>

class FixApplication : public FIX::Application
{
	public:
		FixApplication();
		FixApplication(Nan::Persistent<v8::Object>* callbacks, std::unordered_set<std::string>* callbackRegistry);
		~FixApplication();
		void setLogonProvider(FixLoginProvider* logonProvider);
		void setCredentials(fix_credentials* credentials);
		void setCallbacks(Nan::Persistent<v8::Object>* callbacks);
		void setCallbackRegistry(std::unordered_set<std::string>* callbackRegistry);

	protected:
		Nan::Persistent<v8::Object>* mCallbacks;
		std::unordered_set<std::string>* mCallbackRegistry;
		fix_credentials* mCredentials = NULL;
		FixLoginProvider* mLoginProvider = NULL;

		virtual void onCreate( const FIX::SessionID& sessionID );
		virtual void onLogon( const FIX::SessionID& sessionID );
		virtual void onLogout( const FIX::SessionID& sessionID );

		virtual void toAdmin( FIX::Message& message, const FIX::SessionID& sessionID);
		virtual void fromAdmin( const FIX::Message& message, const FIX::SessionID& sessionID )
		throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon );

		virtual void toApp( FIX::Message&, const FIX::SessionID& ) throw( FIX::DoNotSend );
		virtual void fromApp( const FIX::Message& message, const FIX::SessionID& sessionID )
		throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType );
		virtual void dispatchEvent(std::string eventName, const FIX::Message& message, const FIX::SessionID& sessionID);
		virtual void dispatchEvent(std::string eventName, const FIX::SessionID& sessionID);
};

#endif
