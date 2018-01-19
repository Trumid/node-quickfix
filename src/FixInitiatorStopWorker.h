
#ifndef FIXSTOPWORKER_H_
#define FIXSTOPWORKER_H_

#include "quickfix/SocketInitiator.h"

#include <node.h>
#include <nan.h>

using namespace v8;
using namespace node;

class FixInitiatorStopWorker : public Nan::AsyncWorker
{
	public:
		FixInitiatorStopWorker(Nan::Callback *callback, FIX::Initiator* initiator)
			: Nan::AsyncWorker(callback), initiator(initiator) {}
		~FixInitiatorStopWorker() {}

		void Execute();
		void HandleOKCallback();

	private:
		FIX::Initiator* initiator;
};


#endif /* FIXSTOPWORKER_H_ */
