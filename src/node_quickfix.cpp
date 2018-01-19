#include <v8.h>
#include <node.h>
#include "FixInitiator.h"
#include "FixAcceptor.h"
#include "FixSession.h"

using namespace v8;

NAN_MODULE_INIT(InitAll) {
	FixLoginProvider::Init(target);
	FixInitiator::Init(target);
	FixAcceptor::Init(target);
	FixSession::Initialize();
}

// Register the module with node.
NODE_MODULE(NodeQuickfix, InitAll)
