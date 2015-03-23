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

class FixApplication : public FIX::Application
{
	public:
		FixApplication();
		FixApplication(v8::Persistent<v8::Object>* callbacks);
		~FixApplication();
		void setLogonProvider(FixLoginProvider* logonProvider);
		void setCredentials(fix_credentials* credentials);

	private:
		v8::Persistent<v8::Object>* mCallbacks;
		fix_credentials* mCredentials = NULL;
		FixLoginProvider* mLoginProvider = NULL;

		void onCreate( const FIX::SessionID& sessionID );
		void onLogon( const FIX::SessionID& sessionID );
		void onLogout( const FIX::SessionID& sessionID );
		
		void toAdmin( FIX::Message& message, const FIX::SessionID& sessionID);
		void fromAdmin( const FIX::Message& message, const FIX::SessionID& sessionID )
		throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon );

		void toApp( FIX::Message&, const FIX::SessionID& ) throw( FIX::DoNotSend );
		void fromApp( const FIX::Message& message, const FIX::SessionID& sessionID )
		throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType );
};

#endif
