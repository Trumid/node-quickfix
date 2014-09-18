
#ifndef FIXACCEPTORSTARTWORKER_H_
#define FIXACCEPTORSTARTWORKER_H_

#include "quickfix/SocketAcceptor.h"

#include <node.h>
#include <nan.h>

using namespace v8;
using namespace node;

class FixAcceptorStartWorker : public NanAsyncWorker
{
	public:
		FixAcceptorStartWorker(NanCallback *callback, FIX::SocketAcceptor* acceptor)
			: NanAsyncWorker(callback), acceptor(acceptor) {}
		~FixAcceptorStartWorker() {}

		void Execute();
		void HandleOKCallback();

	private:
		FIX::SocketAcceptor* acceptor;
};


#endif /* FIXACCEPTORSTARTWORKER_H_ */
