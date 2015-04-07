#include <v8.h>
#include <node.h>
#include "FixInitiator.h"
#include "FixAcceptor.h"

using namespace v8;

NAN_METHOD(Initiator) {
  NanScope();
  FixInitiator::New(args);
}

NAN_METHOD(Acceptor) {
  NanScope();
  FixAcceptor::New(args);
}

NAN_METHOD(LoginProvider) {
  NanScope();
  FixLoginProvider::New(args);
}

void init(Handle<Object> target) {
	FixLoginProvider::Initialize(target);
	FixInitiator::Initialize(target);
	FixAcceptor::Initialize(target);
}

// Register the module with node.
NODE_MODULE(node_quickfix, init)
