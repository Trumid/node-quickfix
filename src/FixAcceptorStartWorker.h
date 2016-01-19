
#ifndef FIXACCEPTORSTARTWORKER_H_
#define FIXACCEPTORSTARTWORKER_H_

#include "quickfix/ThreadedSocketAcceptor.h"

#include <node.h>
#include <nan.h>

using namespace v8;
using namespace node;

class FixAcceptorStartWorker : public Nan::AsyncWorker
{
	public:
		FixAcceptorStartWorker(Nan::Callback *callback, FIX::ThreadedSocketAcceptor* acceptor)
			: Nan::AsyncWorker(callback), acceptor(acceptor) {}
		~FixAcceptorStartWorker() {}

		void Execute();
		void HandleOKCallback();

	private:
		FIX::ThreadedSocketAcceptor* acceptor;
};


#endif /* FIXACCEPTORSTARTWORKER_H_ */
