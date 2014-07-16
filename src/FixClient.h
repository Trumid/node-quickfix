#ifndef FIXCLIENT_H
#define FIXCLIENT_H

#include <node.h>
#include "FixApplication.h"
#include "FixEventHandler.h"
#include "quickfix/SocketInitiator.h"

using namespace v8;

class FixClient : public node::ObjectWrap {
 public:
  static void Init();
  static Handle<Value> NewInstance(const Arguments& args);
  void setSessionId(FIX::SessionID sessionId);
  void setInitiator(FIX::SocketInitiator* initiator);

 private:
  explicit FixClient(FixApplication application);
  ~FixClient();
  static Handle<Value> New(const Arguments& args);
  static Persistent<Function> constructor;
  FixApplication mApplication;
  FIX::SessionID mSessionId;
  FIX::SocketInitiator* mInitiator;
  static Handle<Value> start(const Arguments& args);
  static Handle<Value> send(const Arguments& args);
  static Handle<Value> stop(const Arguments& args);
  static FIX::Message* convertToFixMessage(const Handle<Object>);
};

#endif
