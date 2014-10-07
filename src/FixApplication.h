#ifndef APPLICATION_H
#define APPLICATION_H

#include <v8.h>
#include <node.h>
#include <nan.h>

#include "quickfix/Application.h"
#include "quickfix/SessionID.h"
#include "quickfix/Message.h"
#include "FixLoginProvider.h"
#include "FixEvent.h"
#include "FixLogonEvent.h"
#include "FixCredentials.h"


class FixApplication : public FIX::Application
{
	public:
		FixApplication();
		FixApplication(uv_async_t* handle, uv_async_t* logonHandle, v8::Persistent<v8::Object>* callbacks);
		~FixApplication();
		void setLogonProvider(FixLoginProvider* logonProvider);
		void setCredentials(fix_credentials* credentials);

	private:
		uv_async_t* mAsyncHandle;
		uv_async_t* mLogonHandle;
		v8::Persistent<v8::Object>* mCallbacks;
		fix_credentials* mCredentials = NULL;
		FixLoginProvider* mLoginProvider = NULL;

		void onCreate( const FIX::SessionID& ) {}
		void onLogon( const FIX::SessionID& sessionID );
		void onLogout( const FIX::SessionID& sessionID );
		void toAdmin( FIX::Message& message, const FIX::SessionID& sessionId) {
			std::cout << "FIX toAdmin " << std::endl;

			if(strcmp(message.getHeader().getField(35).c_str(), "A") == 0 && mCredentials != NULL) {
				message.setField(553, mCredentials->username.c_str());
				message.setField(554, mCredentials->password.c_str());
			}

			fix_event_t *data = new fix_event_t;
			data->eventName = std::string("toAdmin");
			data->sessionId = &sessionId;
			data->callbacks = mCallbacks;
			data->message = new FIX::Message(message);
			mAsyncHandle->data = data;

			uv_async_send(mAsyncHandle);
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

		  std::cout << "FIX fromAdmin " << std::endl;

		  fix_event_t *data = new fix_event_t;
		  data->eventName = std::string("fromAdmin");
		  data->sessionId = &sessionId;
		  data->callbacks = mCallbacks;
		  data->message = new FIX::Message(message);
		  mAsyncHandle->data = data;

		  uv_async_send(mAsyncHandle);
		}
		void fromApp( const FIX::Message& message, const FIX::SessionID& sessionID )
		throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType );
};

#endif
