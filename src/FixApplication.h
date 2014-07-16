#ifndef APPLICATION_H
#define APPLICATION_H

#include <node.h>
#include "quickfix/Application.h"
#include "quickfix/SessionID.h"
#include "quickfix/Message.h"
#include "FixEventHandler.h"

class FixApplication :
      public FIX::Application
{
	public:
		FixApplication();
		FixApplication(FixEventHandler* handler, uv_async_t* handle);
		~FixApplication();

	private:
	  FixEventHandler* mEventHandler;
	  uv_async_t* mAsyncHandle;
	  void onCreate( const FIX::SessionID& ) {}
	  void onLogon( const FIX::SessionID& sessionID );
	  void onLogout( const FIX::SessionID& sessionID );
	  void toAdmin( FIX::Message& message, const FIX::SessionID& sessionId) {
		  std::cout <<"Message sent in toAdmin:" << std::endl;
		  std::cout << message.toString() << std::endl;
	  }
	  void toApp( FIX::Message&, const FIX::SessionID& )
	  throw( FIX::DoNotSend );
	  void fromAdmin( const FIX::Message& message, const FIX::SessionID& sessionId )
	  throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon ) {
		  std::cout <<"Message received in fromAdmin:" << std::endl;
		  std::cout << message.toString() << std::endl;
	  }
	  void fromApp( const FIX::Message& message, const FIX::SessionID& sessionID )
	  throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType );

};

#endif
