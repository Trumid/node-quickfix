#include <v8.h>
#include <node.h>
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

void init(Handle<Object> target) {
	FixInitiator::Initialize(target);
	FixAcceptor::Initialize(target);
}

// Register the module with node.
NODE_MODULE(node_quickfix, init)
