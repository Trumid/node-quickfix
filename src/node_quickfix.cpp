#include <node/v8.h>
#include <node/node.h>
#include "FixClient.h"

using namespace v8;

Handle<Value> Client(const Arguments& args) {
  HandleScope scope;
  return scope.Close(FixClient::NewInstance(args));
}

void init(Handle<Object> target) {
	FixClient::Init();
	target->Set(String::NewSymbol("Client"),
      FunctionTemplate::New(Client)->GetFunction());
}

// Register the module with node.
NODE_MODULE(node_quickfix, init)
