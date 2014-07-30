
#include "quickfix/FileStore.h"
#include "quickfix/FileLog.h"
#include "quickfix/Initiator.h"
#include "quickfix/SocketInitiator.h"
#include "quickfix/Session.h"
#include "quickfix/SessionSettings.h"

#include "FixInitiator.h"
#include "FixApplication.h"
#include "FixEvent.h"

#include "FixInitiatorStartWorker.h"
#include "FixSendWorker.h"
#include "FixInitiatorStopWorker.h"

//#include "closure.h"

Persistent<Function> FixInitiator::constructor;


/*
 * Node API
 */

void FixInitiator::Initialize(Handle<Object> target) {
  NanScope();

  Local<FunctionTemplate> ctor = NanNew<FunctionTemplate>(FixInitiator::New);

  // TODO:: Figure out what the compile error is with this
  //NanAssignPersistent(constructor, ctor);

  ctor->InstanceTemplate()->SetInternalFieldCount(1);
  ctor->SetClassName(NanNew("FixInitiator"));

  NODE_SET_PROTOTYPE_METHOD(ctor, "start", start);
  NODE_SET_PROTOTYPE_METHOD(ctor, "send", send);
  NODE_SET_PROTOTYPE_METHOD(ctor, "stop", stop);

  target->Set(NanNew("FixInitiator"), ctor->GetFunction());
}

NAN_METHOD(FixInitiator::New) {
	NanScope();

	String::Utf8Value propertiesFile(args[0]);
	FixInitiator *initiator = new FixInitiator(*propertiesFile);

	initiator->Wrap(args.This());
	initiator->mCallbacks = Persistent<Object>::New( args[1]->ToObject() );

	uv_async_init(uv_default_loop(), &initiator->mAsyncFIXEvent, handleFixEvent);

	NanReturnValue(args.This());
}

NAN_METHOD(FixInitiator::start) {
	NanScope();

	FixInitiator* instance = ObjectWrap::Unwrap<FixInitiator>(args.This());

	NanCallback *callback = new NanCallback(args[0].As<Function>());

	NanAsyncQueueWorker(new FixInitiatorStartWorker(callback, instance->mInitiator));

	NanReturnUndefined();
}

NAN_METHOD(FixInitiator::send) {
	NanScope();

	FixInitiator* instance = ObjectWrap::Unwrap<FixInitiator>(args.This());

	Local<Object> message = args[0]->ToObject();
	NanCallback *callback = new NanCallback(args[1].As<Function>());

	FIX::Message* fixMessage = new FIX::Message();
	js2Fix(fixMessage, message);

	NanAsyncQueueWorker(new FixSendWorker(callback, fixMessage));

	NanReturnUndefined();
}

NAN_METHOD(FixInitiator::stop) {
	NanScope();
	FixInitiator* instance = ObjectWrap::Unwrap<FixInitiator>(args.This());

	NanCallback *callback = new NanCallback(args[0].As<Function>());

	NanAsyncQueueWorker(new FixInitiatorStopWorker(callback, instance->mInitiator));

	NanReturnUndefined();
}

/*
 * Implementation of FixInitiator Class
 */

FixInitiator::FixInitiator(const char* propertiesFile): ObjectWrap() {
	mSettings = FIX::SessionSettings(propertiesFile);
	FIX::FileStoreFactory storeFactory(mSettings);
	FIX::FileLogFactory logFactory(mSettings);

	mFixApplication = new FixApplication(&mAsyncFIXEvent, &mCallbacks);

	mInitiator = new FIX::SocketInitiator(*mFixApplication, storeFactory, mSettings);
}

FixInitiator::~FixInitiator() {
	uv_close((uv_handle_t*) &mAsyncFIXEvent, NULL);
}

void FixInitiator::handleFixEvent(uv_async_t *handle, int status) {
	NanScope();

	fix_event_t* event = (fix_event_t*)handle->data;

	Local<String> eventName = NanNew<String>(event->eventName.c_str());

	Local<Function> callback = Local<Function>::Cast((*event->callbacks)->Get(eventName));

	if(event->message != NULL){
		Local<Object> msg = NanNew<Object>();

		fix2Js(msg, event->message);

		Local<Value> argv[] = {
				msg,
				NanNew<String>(event->sessionId->toString().c_str())
		};
		NanMakeCallback(NanGetCurrentContext()->Global(), callback, 2, argv);
	} else {
		Local<Value> argv[] = {
				NanNew<String>(event->sessionId->toString().c_str())
		};

		NanMakeCallback(NanGetCurrentContext()->Global(), callback, 1, argv);
	}

}


void FixInitiator::js2Fix(FIX::Message* message, Local<Object> msg) {

	Local<Object> header = Local<Object>::Cast(msg->Get(String::New("header")));
	Local<Object> tags = Local<Object>::Cast(msg->Get(String::New("message")));

	FIX::Header &msgHeader = message->getHeader();
	FIX::Trailer &msgTrailer = message->getTrailer();

	Local<Array> headerTags = header->GetPropertyNames();
	for(int i=0; i < (int)headerTags->Length(); i++) {
		String::Utf8Value value(header->Get(headerTags->Get(i))->ToString());

		msgHeader.setField(
				headerTags->Get(i)->Int32Value(),
				std::string(*value)
		);
	}

	Local<Array> msgTags = tags->GetPropertyNames();
	for(int i=0; i < (int)msgTags->Length(); i++) {
		String::Utf8Value value(tags->Get(msgTags->Get(i))->ToString());

		message->setField(
				msgTags->Get(i)->Int32Value(),
				std::string(*value)
		);
	}

	Local<String> groupKey = String::New("groups");
	if(msg->Has(groupKey))
	{
		Local<Array> groups = Local<Array>::Cast(msg->Get(groupKey));
		for(int i=0; i<(int)groups->Length(); i++) {
			Local<Object> groupObj = groups->Get(i)->ToObject();
			FIX::Group* group = new FIX::Group(
					groupObj->Get(String::New("index"))->ToInteger()->Value(),
					groupObj->Get(String::New("delim"))->ToInteger()->Value());

			Local<Array> groupEntries = Local<Array>::Cast(groupObj->Get(String::New("entries")));
			for(int j=0; j<(int)groupEntries->Length(); j++) {
				Local<Object> entry = groupEntries->Get(j)->ToObject();
				Local<Array> entryTags = entry->GetPropertyNames();
				for(int k=0; k<(int)entryTags->Length(); k++) {
					Local<String> prop = entryTags->Get(k)->ToString();
					String::Utf8Value keyStr(prop->ToString());

					String::Utf8Value valueStr(entry->Get(prop)->ToString());
					group->setField(atoi(std::string(*keyStr).c_str()), std::string(*valueStr));
				}
			}
			message->addGroup(*group);
		}
	}

	Local<String> trailerKey = String::New("trailer");
	if(msg->Has(trailerKey))
	{
		Local<Object> trailer = Local<Object>::Cast(msg->Get(trailerKey));
		Local<Array> trailerTags = trailer->GetPropertyNames();
		for(int i=0; i<(int)trailerTags->Length(); i++) {
			Local<String> prop = trailerTags->Get(i)->ToString();
			String::Utf8Value keyStr(prop->ToString());

			String::Utf8Value valueStr(trailer->Get(prop)->ToString());

			msgTrailer.setField(atoi(std::string(*keyStr).c_str()), std::string(*valueStr));
		}
	}
}

 Handle<Value> sessionIdToJs(FIX::SessionID* sessionId) {
	Handle<Object> session = Object::New();
	session->Set(String::New("beginString"), String::New(sessionId->getBeginString().getString().c_str()));
	session->Set(String::New("senderCompID"), String::New(sessionId->getSenderCompID().getString().c_str()));
	session->Set(String::New("targetCompID"), String::New(sessionId->getTargetCompID().getString().c_str()));
	session->Set(String::New("sessionQualifier"), String::New(sessionId->getSessionQualifier().c_str()));

	return session;
}

void FixInitiator::fix2Js(Local<Object> msg, const FIX::Message* message) {
	Local<Object> header = NanNew<Object>();
	Local<Object> tags = NanNew<Object>();
	Local<Object> trailer = NanNew<Object>();
	Local<Object> groups = NanNew<Object>();

	FIX::Header messageHeader = message->getHeader();
	FIX::Trailer messageTrailer = message->getTrailer();

	for(FIX::FieldMap::iterator it = messageHeader.begin(); it != messageHeader.end(); ++it)
	{
		header->Set(Integer::New(it->first), String::New(it->second.getString().c_str()));
	}

	for(FIX::FieldMap::iterator it = message->begin(); it != message->end(); ++it)
	{
		tags->Set(Integer::New(it->first), String::New(it->second.getString().c_str()));
	}

	for(FIX::FieldMap::iterator it = messageTrailer.begin(); it != messageTrailer.end(); ++it)
	{
		trailer->Set(Integer::New(it->first), String::New(it->second.getString().c_str()));
	}

	for(FIX::FieldMap::g_iterator it = message->g_begin(); it != message->g_end(); ++it)
	{
		std::vector< FIX::FieldMap* > groupVector = it->second;
		Handle<Array> groupList = Array::New(groupVector.size());
		int i=0;
		for(std::vector< FIX::FieldMap* >::iterator v_it = groupVector.begin(); v_it != groupVector.end(); ++v_it)
		{
			Handle<Object> groupEntry = Object::New();
			FIX::FieldMap* fields = *v_it;
			for(FIX::FieldMap::iterator field_it = fields->begin(); field_it != fields->end(); ++field_it)
			{
				groupEntry->Set(Integer::New(field_it->first), String::New(field_it->second.getString().c_str()));
			}
			groupList->Set(i, groupEntry);
			i++;
		}

		groups->Set(Integer::New(it->first), groupList);
	}

	msg->Set(String::New("header"), header);
	msg->Set(String::New("tags"), tags);
	msg->Set(String::New("trailer"), trailer);
	msg->Set(String::New("groups"), groups);
}


