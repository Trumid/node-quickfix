#ifndef APPLICATION_H
#define APPLICATION_H

#include <v8.h>
#include <node.h>
#include <nan.h>

#include "quickfix/Application.h"
#include "quickfix/SessionID.h"
#include "quickfix/Message.h"
#include "FixLoginProvider.h"
#include "FixLogonEvent.h"


class FixApplication : public FIX::Application
{
	public:
		FixApplication();
		FixApplication(uv_async_t* handle, uv_async_t* logonHandle, v8::Persistent<v8::Object>* callbacks);
		~FixApplication();
		void setLogonProvider(FixLoginProvider* logonProvider);

	private:
		uv_async_t* mAsyncHandle;
		uv_async_t* mLogonHandle;
		v8::Persistent<v8::Object>* mCallbacks;
		FixLoginProvider* mLoginProvider = NULL;

		void onCreate( const FIX::SessionID& ) {}
		void onLogon( const FIX::SessionID& sessionID );
		void onLogout( const FIX::SessionID& sessionID );
		void toAdmin( FIX::Message& message, const FIX::SessionID& sessionId) {
		}
		void toApp( FIX::Message&, const FIX::SessionID& ) throw( FIX::DoNotSend );
		void fromAdmin( const FIX::Message& message, const FIX::SessionID& sessionId )
		throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon ) {
		  if(strcmp(message.getHeader().getField(35).c_str(), "A") == 0 && mLoginProvider != NULL) {
			  fix_logon_t *data = new fix_logon_t;
			  data->sessionId = &sessionId;
			  data->logon = mLoginProvider->getLogon();
			  data->message = new FIX::Message(message);
			  mLogonHandle->data = data;

			  uv_async_send(mLogonHandle);

			  while(!mLoginProvider->getIsFinished()) {
			  }

			  if(!mLoginProvider->getIsLoggedOn()) {
				  throw FIX::RejectLogon();
			  }
		  }
		}
		void fromApp( const FIX::Message& message, const FIX::SessionID& sessionID )
		throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType );
};

#endif
