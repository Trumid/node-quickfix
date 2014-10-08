/*
 * FixMessageUtil.h
 *
 *  Created on: Sep 8, 2014
 *      Author: kdeol
 */

#ifndef FIXMESSAGEUTIL_H_
#define FIXMESSAGEUTIL_H_

#include <v8.h>
#include <node.h>
#include <nan.h>

#include "FixEvent.h"
#include "quickfix/SessionID.h"
#include "quickfix/Message.h"

using namespace v8;
using namespace node;

class FixMessageUtil {
public:
	FixMessageUtil();
	virtual ~FixMessageUtil();

	static void handleFixEvent(uv_async_t *handle, int status) {
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

	static void js2Fix(FIX::Message* message, Local<Object> msg) {

		Local<Object> header = Local<Object>::Cast(msg->Get(String::New("header")));
		Local<Object> tags = Local<Object>::Cast(msg->Get(String::New("tags")));

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


	static void fix2Js(Local<Object> msg, const FIX::Message* message) {
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

	static Local<Value> sessionIdToJs(const FIX::SessionID* sessionId) {
		Local<Object> session = Object::New();
		session->Set(String::New("beginString"), String::New(sessionId->getBeginString().getString().c_str()));
		session->Set(String::New("senderCompID"), String::New(sessionId->getSenderCompID().getString().c_str()));
		session->Set(String::New("targetCompID"), String::New(sessionId->getTargetCompID().getString().c_str()));
		session->Set(String::New("sessionQualifier"), String::New(sessionId->getSessionQualifier().c_str()));

		return session;
	}

	static FIX::SessionID jsToSessionId(Local<Object> sessionId) {
		String::Utf8Value beginString(sessionId->Get(String::New("beginString"))->ToString());
		String::Utf8Value senderCompId(sessionId->Get(String::New("senderCompID"))->ToString());
		String::Utf8Value targetCompId(sessionId->Get(String::New("targetCompID"))->ToString());
		return FIX::SessionID(std::string(*beginString),
				std::string(*senderCompId),
				std::string(*targetCompId));
	}
};

#endif /* FIXMESSAGEUTIL_H_ */
