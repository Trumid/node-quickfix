
#ifndef FIXSTARTWORKER_H_
#define FIXSTARTWORKER_H_

#include "quickfix/SocketInitiator.h"

#include <node.h>
#include <nan.h>

using namespace v8;
using namespace node;

class FixStartWorker : public NanAsyncWorker
{
	public:
		FixStartWorker(NanCallback *callback, FIX::SocketInitiator* initiator)
			: NanAsyncWorker(callback), initiator(initiator) {}
		~FixStartWorker() {}

		void Execute();
		void HandleOKCallback();

	private:
		FIX::SocketInitiator* initiator;
};


#endif /* FIXSTARTWORKER_H_ */
