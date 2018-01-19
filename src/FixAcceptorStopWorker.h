
#ifndef FIXACCEPTORSTOPWORKER_H_
#define FIXACCEPTORSTOPWORKER_H_

#include "quickfix/SocketAcceptor.h"

#include <node.h>
#include <nan.h>

using namespace v8;
using namespace node;

class FixAcceptorStopWorker : public Nan::AsyncWorker
{
	public:
		FixAcceptorStopWorker(Nan::Callback *callback, FIX::Acceptor* acceptor)
			: Nan::AsyncWorker(callback), acceptor(acceptor) {}
		~FixAcceptorStopWorker() {}

		void Execute();
		void HandleOKCallback();

	private:
		FIX::Acceptor* acceptor;
};


#endif /* FIXACCEPTORSTOPWORKER_H_ */
