#include <node.h>
#include "quickfix/FileStore.h"
#include "quickfix/FileLog.h"
#include "quickfix/SocketInitiator.h"
#include "quickfix/Session.h"
#include "quickfix/SessionSettings.h"
#include "quickfix/Application.h"
#include "FixInitiator.h"
#include "FixApplication.h"
#include "FixEventHandler.h"

using namespace v8;
using namespace std;

Persistent<Function> FixInitiator::constructor;
FIX::SessionID mSessionId;
uv_async_t initiatorAsync;
uv_async_t appAsync;

FixInitiator::FixInitiator(FixApplication application) :
		mApplication(application) {
}

FixInitiator::~FixInitiator() {
}

void FixInitiator::Init() {
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

Handle<Value> FixInitiator::New(const Arguments& args) {
  HandleScope scope;

  if (args.IsConstructCall()) {
    // Invoked as constructor: `new MyObject(...)`
	FixEventHandler* handler = new FixEventHandler(args);
	FixInitiator* obj = new FixInitiator(FixApplication::FixApplication(handler, &appAsync));
    obj->Wrap(args.This());
    return scope.Close(args.This());
  } else {
    // Invoked as plain function `MyObject(...)`, turn into construct call.
    const int argc = 1;
    Local<Value> argv[argc] = { args[0] };
    return scope.Close(constructor->NewInstance(argc, argv));
  }
}

Handle<Value> FixInitiator::NewInstance(const Arguments& args) {
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
	FixInitiator* client;
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

Handle<Value> FixInitiator::start(const Arguments& args) {
	try{
	HandleScope scope;

	FixInitiator* obj = ObjectWrap::Unwrap<FixInitiator>(args.This());

	FixApplication app = obj->mApplication;
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
	} catch (FIX::FieldConvertError& e) {
		printf(e.what());
	}
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

Handle<Value> FixInitiator::send(const Arguments& args) {
	HandleScope scope;
	printf("In send()\n");
	//Add validation on args here!
	FixInitiator* client = ObjectWrap::Unwrap<FixInitiator>(args.This());
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

Handle<Value> FixInitiator::stop(const Arguments& args) {
	HandleScope scope;
	FixInitiator* client = ObjectWrap::Unwrap<FixInitiator>(args.This());
	FIX::SocketInitiator* initiator = client->mInitiator;
	if(initiator == NULL)
	{
		cout << "Trying to stop an Initiator that is not started!" << endl;
	} else {
		initiator->stop(true);
		uv_close((uv_handle_t*)&appAsync, NULL);
		//delete initiator;
	}
	printf("Initiator stopped!");
	return Undefined();
}

void FixInitiator::setSessionId(FIX::SessionID sessionId)
{
	mSessionId = sessionId;
}

void FixInitiator::setInitiator(FIX::SocketInitiator* initiator)
{
	mInitiator = initiator;
}

FIX::Message* FixInitiator::convertToFixMessage(Handle<Object> msg) {
	FIX::Message *message = new FIX::Message;
	Local<Object> header = Local<Object>::Cast(msg->Get(String::New("header")));
	FIX::Header &msgHeader = message->getHeader();

	Local<Array> headerTags = header->GetPropertyNames();
	for(int i=0; i<(int)headerTags->Length(); i++) {
		Local<String> prop = headerTags->Get(i)->ToString();
		String::Utf8Value keyStr(prop->ToString());

		String::Utf8Value valueStr(header->Get(prop)->ToString());

		msgHeader.setField(atoi(string(*keyStr).c_str()), string(*valueStr));
	}

	Local<Object> tags = Local<Object>::Cast(msg->Get(String::New("message")));
	Local<Array> msgTags = tags->GetPropertyNames();
	for(int i=0; i<(int)msgTags->Length(); i++) {
		Local<String> prop = msgTags->Get(i)->ToString();
		String::Utf8Value keyStr(prop->ToString());

		String::Utf8Value valueStr(tags->Get(prop)->ToString());
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






