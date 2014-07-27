#include <node/v8.h>
#include <node/node.h>
#include "FixInitiator.h"

using namespace v8;

Handle<Value> Initiator(const Arguments& args) {
  HandleScope scope;
  return scope.Close(FixInitiator::New(args));
}

void init(Handle<Object> target) {
	FixInitiator::Initialize(target);
}

// Register the module with node.
NODE_MODULE(node_quickfix, init)
