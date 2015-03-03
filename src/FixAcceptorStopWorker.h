
#ifndef FIXACCEPTORSTOPWORKER_H_
#define FIXACCEPTORSTOPWORKER_H_

#include "quickfix/SocketAcceptor.h"

#include <node.h>
#include <nan.h>

using namespace v8;
using namespace node;

class FixAcceptorStopWorker : public NanAsyncWorker
{
	public:
		FixAcceptorStopWorker(NanCallback *callback, FIX::ThreadedSocketAcceptor* acceptor)
			: NanAsyncWorker(callback), acceptor(acceptor) {}
		~FixAcceptorStopWorker() {}

		void Execute();
		void HandleOKCallback();

	private:
		FIX::ThreadedSocketAcceptor* acceptor;
};


#endif /* FIXACCEPTORSTOPWORKER_H_ */
