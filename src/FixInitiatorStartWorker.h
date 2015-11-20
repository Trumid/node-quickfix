
#ifndef FIXSTARTWORKER_H_
#define FIXSTARTWORKER_H_

#include "quickfix/SocketInitiator.h"

#include <node.h>
#include <nan.h>

using namespace v8;
using namespace node;

class FixInitiatorStartWorker : public Nan::AsyncWorker
{
	public:
		FixInitiatorStartWorker(Nan::Callback *callback, FIX::SocketInitiator* initiator)
			: Nan::AsyncWorker(callback), initiator(initiator) {}
		~FixInitiatorStartWorker() {}

		void Execute();
		void HandleOKCallback();

	private:
		FIX::SocketInitiator* initiator;
};


#endif /* FIXSTARTWORKER_H_ */
