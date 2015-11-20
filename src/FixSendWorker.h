
#ifndef FIXSENDWORKER_H_
#define FIXSENDWORKER_H_

#include "quickfix/Message.h"
#include "quickfix/SocketInitiator.h"

#include <node.h>
#include <nan.h>

using namespace v8;
using namespace node;

class FixSendWorker : public Nan::AsyncWorker
{
	public:
		FixSendWorker(Nan::Callback *callback, FIX::Message* message)
			: Nan::AsyncWorker(callback), message(message) {}
		~FixSendWorker() {
			if(message) {
				delete message;
			}
		}

		void Execute();
		void HandleOKCallback();

	private:
		FIX::Message* message;
};


#endif /* FIXSENDWORKER_H_ */
