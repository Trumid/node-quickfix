
#ifndef FIXSTOPWORKER_H_
#define FIXSTOPWORKER_H_

#include "quickfix/SocketInitiator.h"

#include <node.h>
#include <nan.h>

using namespace v8;
using namespace node;

class FixStopWorker : public NanAsyncWorker
{
	public:
		FixStopWorker(NanCallback *callback, FIX::SocketInitiator* initiator)
			: NanAsyncWorker(callback), initiator(initiator) {}
		~FixStopWorker() {}

		void Execute();
		void HandleOKCallback();

	private:
		FIX::SocketInitiator* initiator;
};


#endif /* FIXSTOPWORKER_H_ */
