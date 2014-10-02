#include <node/v8.h>
#include <node/node.h>
#include "FixInitiator.h"
#include "FixAcceptor.h"

using namespace v8;

Handle<Value> Initiator(const Arguments& args) {
  HandleScope scope;
  return scope.Close(FixInitiator::New(args));
}

Handle<Value> Acceptor(const Arguments& args) {
  HandleScope scope;
  return scope.Close(FixAcceptor::New(args));
}

Handle<Value> LoginProvider(const Arguments& args) {
  HandleScope scope;
  return scope.Close(FixLoginProvider::New(args));
}

void init(Handle<Object> target) {
	FixLoginProvider::Initialize(target);
	FixInitiator::Initialize(target);
	FixAcceptor::Initialize(target);
}

// Register the module with node.
NODE_MODULE(node_quickfix, init)
