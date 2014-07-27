#ifndef APPLICATION_H
#define APPLICATION_H

#include <v8.h>
#include <node.h>
#include <nan.h>

#include "quickfix/Application.h"
#include "quickfix/SessionID.h"
#include "quickfix/Message.h"


class FixApplication : public FIX::Application
{
	public:
		FixApplication();
		FixApplication(uv_async_t* handle, v8::Persistent<v8::Object>* callbacks);
		~FixApplication();

	private:
		uv_async_t* mAsyncHandle;
		v8::Persistent<v8::Object>* mCallbacks;

		void onCreate( const FIX::SessionID& ) {}
		void onLogon( const FIX::SessionID& sessionID );
		void onLogout( const FIX::SessionID& sessionID );
		void toAdmin( FIX::Message& message, const FIX::SessionID& sessionId) {
		  //std::cout <<"Message sent in toAdmin:" << std::endl;
		  //std::cout << message.toString() << std::endl;
		}
		void toApp( FIX::Message&, const FIX::SessionID& ) throw( FIX::DoNotSend );
		void fromAdmin( const FIX::Message& message, const FIX::SessionID& sessionId )
		throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon ) {
		  //std::cout <<"Message received in fromAdmin:" << std::endl;
		  //std::cout << message.toString() << std::endl;
		}
		void fromApp( const FIX::Message& message, const FIX::SessionID& sessionID )
		throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType );
};

#endif
