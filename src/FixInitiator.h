#ifndef FIXINITIATOR_H
#define FIXINITIATOR_H

#include <v8.h>
#include <node.h>
#include <nan.h>

#include "FixConnection.h"
#include "FixInitiator.h"
#include "FixApplication.h"

#include "quickfix/Application.h"
#include "quickfix/SessionID.h"
#include "quickfix/Message.h"
#include "quickfix/SocketInitiator.h"

using namespace v8;
using namespace node;

class FixInitiator : public FixConnection {
 public:
  //virtual static Nan::Persistent<Function> constructor;
  static void Initialize(Handle<Object> target);
  static NAN_METHOD(New);
  static NAN_METHOD(start);
  static NAN_METHOD(send);
  static NAN_METHOD(sendRaw);
  static NAN_METHOD(stop);
  static NAN_METHOD(isLoggedOn);
  static NAN_METHOD(getSessions);
  static NAN_METHOD(getSession);

  FixInitiator(FIX::SessionSettings settings, std::string storeFactory);

 private:
  ~FixInitiator();
  FIX::SocketInitiator* mInitiator;
  static void sendAsync(const Nan::FunctionCallbackInfo<v8::Value>& info, FIX::Message* message);
};

#endif
