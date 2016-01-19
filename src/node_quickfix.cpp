#include <v8.h>
#include <node.h>
#include "FixInitiator.h"
#include "FixAcceptor.h"

using namespace v8;

NAN_METHOD(Initiator) {
  Nan::HandleScope scope;
  FixInitiator::New(info);
}

NAN_METHOD(Acceptor) {
  Nan::HandleScope scope;
  FixAcceptor::New(info);
}

NAN_METHOD(LoginProvider) {
  Nan::HandleScope scope;
  FixLoginProvider::New(info);
}

void init(Handle<Object> target) {
	FixLoginProvider::Initialize(target);
	FixInitiator::Initialize(target);
	FixAcceptor::Initialize(target);
}

// Register the module with node.
NODE_MODULE(node_quickfix, init)
