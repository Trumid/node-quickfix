#ifndef FIXINITIATOR_H
#define FIXINITIATOR_H

#include <v8.h>
#include <node.h>
#include <nan.h>

#include "FixInitiator.h"
#include "FixApplication.h"

#include "quickfix/Application.h"
#include "quickfix/SessionID.h"
#include "quickfix/Message.h"
#include "quickfix/SocketInitiator.h"

using namespace v8;
using namespace node;

class FixInitiator : public node::ObjectWrap {
 public:
  static Persistent<Function> constructor;
  static void Initialize(Handle<Object> target);
  static NAN_METHOD(New);
  static NAN_METHOD(start);
  static NAN_METHOD(send);
  static NAN_METHOD(stop);

  static void handleFixEvent(uv_async_t *handle, int status);

  FixInitiator(const char* propertiesFile);

 private:
  ~FixInitiator();

  static void js2Fix(FIX::Message* message, Local<Object> msg);
  static void fix2Js(Local<Object> msg, const FIX::Message* message);

  uv_async_t mAsyncFIXEvent;

  FixApplication* mFixApplication;

  FIX::SessionSettings mSettings;
  Persistent<Object> mCallbacks;
  FIX::SocketInitiator* mInitiator;
};

#endif
