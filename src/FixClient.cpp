#include <node.h>
#include "quickfix/FileStore.h"
#include "quickfix/FileLog.h"
#include "quickfix/SocketInitiator.h"
#include "quickfix/Session.h"
#include "quickfix/SessionSettings.h"
#include "quickfix/Application.h"
#include "FixClient.h"
#include "FixApplication.h"
#include "FixEventHandler.h"

using namespace v8;
using namespace std;

Persistent<Function> FixClient::constructor;
FIX::SessionID mSessionId;
uv_async_t initiatorAsync;
uv_async_t appAsync;

FixClient::FixClient(FixApplication application) :
		mApplication(application) {
}

FixClient::~FixClient() {
}

void FixClient::Init() {
  // Prepare constructor template
  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
  tpl->SetClassName(String::NewSymbol("FixClient"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->PrototypeTemplate()->Set(String::NewSymbol("start"),
		  FunctionTemplate::New(start)->GetFunction());
  tpl->PrototypeTemplate()->Set(String::NewSymbol("send"),
		  FunctionTemplate::New(send)->GetFunction());
  tpl->PrototypeTemplate()->Set(String::NewSymbol("stop"),
		  FunctionTemplate::New(stop)->GetFunction());
  constructor = Persistent<Function>::New(tpl->GetFunction());
}

Handle<Value> FixClient::New(const Arguments& args) {
  HandleScope scope;

  if (args.IsConstructCall()) {
    // Invoked as constructor: `new MyObject(...)`
	FixEventHandler* handler = new FixEventHandler(args);
    FixClient* obj = new FixClient(FixApplication::FixApplication(handler, &appAsync));
    obj->Wrap(args.This());
    return scope.Close(args.This());
  } else {
    // Invoked as plain function `MyObject(...)`, turn into construct call.
    const int argc = 1;
    Local<Value> argv[argc] = { args[0] };
    return scope.Close(constructor->NewInstance(argc, argv));
  }
}

Handle<Value> FixClient::NewInstance(const Arguments& args) {
  HandleScope scope;

  const unsigned argc = 1;
  Handle<Value> argv[argc] = { args[0] };
  Local<Object> instance = constructor->NewInstance(argc, argv);

  return scope.Close(instance);
}

struct InitiatorRequest {
	Persistent<Function> callback;
	std::string propertiesFile;
	FixApplication app;
	FixClient* client;
	FIX::SessionID sessionId;
};

void startInitiator(uv_work_t *req) {
	try
	{
		InitiatorRequest *input = static_cast<InitiatorRequest*>(req->data);
		FixApplication app = input->app;
		FIX::SessionSettings settings(input->propertiesFile);

		FIX::FileStoreFactory storeFactory(settings);
		FIX::FileLogFactory logFactory(settings);
		FIX::SocketInitiator initiator
		  (app, storeFactory, settings, logFactory);

		printf("Starting initiator!\n");
		initiator.start();
		FIX::SessionID sessionId = *initiator.getSessions().begin();
		input->client->setSessionId(sessionId);
		input->client->setInitiator(&initiator);

		initiatorAsync.data = (void*) &input->callback;

		while(!initiator.isLoggedOn()){} //wait until we are logged on
		uv_async_send(&initiatorAsync); //notify that we have finished starting the initiator and are logged on
		while(true){} //keep this thing running until we call stop
	}
	catch(FIX::ConfigError& e)
	{
	    //handle this exception
		std::cout << e.what();
		return;
	}
}

void afterInitiatorStart(uv_work_t *req, int status) {
	HandleScope scope;

	InitiatorRequest *m = (InitiatorRequest *)req->data;
	Handle<Value> argv[0] = {};
	m->callback->Call(Context::GetCurrent()->Global(), 0, argv);
	m->callback.Dispose();

	delete m;
	delete req;
}

void initiatorStartRunCallback(uv_async_t *handle, int status /*UNUSED*/) {
	HandleScope scope;
	Persistent<Function> callback = *((Persistent<Function>*)handle->data);

	Handle<Value> argv[0] = {};
	callback->Call(Context::GetCurrent()->Global(), 0, argv);
	callback.Dispose();
	uv_close((uv_handle_t*) &initiatorAsync, NULL);
}

Handle<Value> FixClient::start(const Arguments& args) {
	HandleScope scope;

	FixClient* obj = ObjectWrap::Unwrap<FixClient>(args.This());

	FixApplication app = obj->mApplication;
	//app.setAsyncHandle(&appAsync);
	String::Utf8Value utf8(args[0]->ToString());
	std::string fileName = string(*utf8);

	uv_work_t *req = new uv_work_t;
	InitiatorRequest *data = new InitiatorRequest;
	data->callback = Persistent<Function>::New(args[1].As<Function>());
	data->propertiesFile = fileName;
	data->app = app;
	data->client = obj;
	req->data = data;

	uv_loop_t* loop = uv_default_loop();
	uv_async_init(loop, &initiatorAsync, initiatorStartRunCallback);
	uv_async_init(loop, &appAsync, FixEventHandler::handleFixEvent);
	uv_queue_work(loop, req, startInitiator, afterInitiatorStart);

	return Undefined();
}

struct SendMessageRequest {
	Persistent<Function> callback;
	FIX::Message* message;
	FIX::SessionID sessionId;
};

void sendMessage(uv_work_t *req) {
	SendMessageRequest *data = static_cast<SendMessageRequest*>(req->data);
	printf("Sending message with sessionId %s \n", data->sessionId.toString().c_str());
	FIX::Session::sendToTarget(*(data->message), data->sessionId);
}

void afterMessageSent(uv_work_t *req, int status) {
	HandleScope scope;
	SendMessageRequest *data = static_cast<SendMessageRequest*>(req->data);
	Handle<Value> argv[0] = {};

	data->callback->Call(Context::GetCurrent()->Global(), 0, argv);
	data->callback.Dispose();
	delete data;
	delete req;
}

Handle<Value> FixClient::send(const Arguments& args) {
	HandleScope scope;
	printf("In send()\n");
	//Add validation on args here!
	FixClient* client = ObjectWrap::Unwrap<FixClient>(args.This());
	uv_work_t *req = new uv_work_t;
	SendMessageRequest *data = new SendMessageRequest;
	data->callback = Persistent<Function>::New(args[1].As<Function>());
	data->message = convertToFixMessage(args[0]->ToObject());
	data->sessionId = client->mSessionId;
	req->data = data;

	printf("Queuing async send thread\n");
	uv_queue_work(uv_default_loop(), req, sendMessage, afterMessageSent);
	//When to close scope vs not? Is this because closing the scope before task finishes running would cause fuckups?
	return Undefined();
}

Handle<Value> FixClient::stop(const Arguments& args) {
	HandleScope scope;
	FixClient* client = ObjectWrap::Unwrap<FixClient>(args.This());
	FIX::SocketInitiator* initiator = client->mInitiator;
	if(initiator == NULL)
	{
		cout << "Trying to stop an Initiator that is not started!" << endl;
	} else {
		initiator->stop(true);
		uv_close((uv_handle_t*)&appAsync, NULL);
		//delete initiator;
	}
	return Undefined();
}

void FixClient::setSessionId(FIX::SessionID sessionId)
{
	mSessionId = sessionId;
}

void FixClient::setInitiator(FIX::SocketInitiator* initiator)
{
	mInitiator = initiator;
}

FIX::Message* FixClient::convertToFixMessage(Handle<Object> msg) {
	FIX::Message *message = new FIX::Message;
	Local<Array> header = Local<Array>::Cast(msg->Get(String::New("header")));
	FIX::Header &msgHeader = message->getHeader();

	for(int i=0; i<(int)header->Length(); i++) {
		Local<Object> field = Local<Object>::Cast(header->Get(i));
		String::Utf8Value keyStr(field->Get(String::New("key"))->ToString());

		String::Utf8Value valueStr(field->Get(String::New("value"))->ToString());

		msgHeader.setField(atoi(string(*keyStr).c_str()), string(*valueStr));
	}

	Local<Array> tags = Local<Array>::Cast(msg->Get(String::New("message")));

	for(int i=0; i<(int)tags->Length(); i++) {
		Local<Object> field = Local<Object>::Cast(tags->Get(i));
		String::Utf8Value keyStr(field->Get(String::New("key"))->ToString());

		String::Utf8Value valueStr(field->Get(String::New("value"))->ToString());
		message->setField(atoi(string(*keyStr).c_str()), string(*valueStr));
	}

	//come back to groups when it needs to be implemented
	Local<String> groupKey = String::New("groups");
	if(msg->Has(groupKey))
	{
		Local<Array> groups = Local<Array>::Cast(groupKey);
	}

	return message;
}






