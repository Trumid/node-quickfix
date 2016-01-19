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

using namespace node;

class FixMessageUtil {
public:
	FixMessageUtil();
	virtual ~FixMessageUtil();

	static void addFixHeader(FIX::Message* message, Local<v8::Object> msg) {

		Local<v8::Object> header = Local<v8::Object>::Cast(msg->Get(Nan::New<String>("header").ToLocalChecked()));
		FIX::Header &msgHeader = message->getHeader();
		Local<v8::Array> headerTags = header->GetPropertyNames();

		for(int i=0; i < (int)headerTags->Length(); i++) {
			String::Utf8Value value(header->Get(headerTags->Get(i))->ToString());
			msgHeader.setField(
					headerTags->Get(i)->Int32Value(),
					std::string(*value)
			);
		}
	}

	static void addFixTags(FIX::FieldMap* map, Local<v8::Object> msg) {

		Local<v8::String> tagsKey = Nan::New<v8::String>("tags").ToLocalChecked();

		if(msg->Has(tagsKey)) {

			Local<v8::Object> tags = Local<v8::Object>::Cast(msg->Get(tagsKey));

			Local<v8::Array> msgTags = tags->GetPropertyNames();

			for(int i=0; i < (int)msgTags->Length(); i++) {
				String::Utf8Value value(tags->Get(msgTags->Get(i))->ToString());


				map->setField(
						msgTags->Get(i)->Int32Value(),
						std::string(*value)
				);
			}

		}

	}

	static void addFixGroups(FIX::FieldMap* map, Local<v8::Object> msg) {
		Local<v8::String> groupKey = Nan::New<v8::String>("groups").ToLocalChecked();

		// TODO: add type checking and dev-helpful error throwing


		if(msg->Has(groupKey)) {


			Local<v8::Array> groups = Local<v8::Array>::Cast(msg->Get(groupKey));

			for(int i = 0; i < (int) groups->Length(); i++) {


				Local<v8::Object> groupObj = groups->Get(i)->ToObject();

				Local<v8::String> delimKey = Nan::New<v8::String>("delim").ToLocalChecked();
				Local<v8::String> indexKey = Nan::New<v8::String>("index").ToLocalChecked();

				if( ! groupObj->Has(indexKey)) {
						Nan::ThrowError("no index property found on object");
				}

				if( ! groupObj->Has(delimKey)) {
						Nan::ThrowError("no delim property found on object");
				}


				Local<v8::String> entriesKey = Nan::New<v8::String>("entries").ToLocalChecked();

				if( ! groupObj->Has(entriesKey)) {
						Nan::ThrowError("no entries property found on object");
				}

				Local<v8::Array> groupEntries = Local<v8::Array>::Cast(groupObj->Get(entriesKey));

				for (int j = 0; j < (int) groupEntries->Length(); j++) {

					Local<v8::Object> entry = groupEntries->Get(j)->ToObject();

					Local<v8::String> tagKey = Nan::New<v8::String>("tags").ToLocalChecked();

					if(entry->Has(groupKey) || entry->Has(tagKey)) {

						FIX::Group* group = new FIX::Group(
							groupObj->Get(indexKey)->ToInteger()->Value(),
							groupObj->Get(delimKey)->ToInteger()->Value());

						addFixTags(group, entry);
						addFixGroups(group, entry);

						map->addGroup(group->field(), *group);

						delete group;

					} else {

						FIX::Group* group = new FIX::Group(
							groupObj->Get(indexKey)->ToInteger()->Value(),
							groupObj->Get(delimKey)->ToInteger()->Value());

						// compat for old, non-nested format

						Local<v8::Array> entryTags = entry->GetPropertyNames();

						for(int k=0; k < (int) entryTags->Length(); k++) {

							Local<v8::String> prop = entryTags->Get(k)->ToString();
							String::Utf8Value keyStr(prop->ToString());
							String::Utf8Value valueStr(entry->Get(prop)->ToString());

							group->setField(atoi(std::string(*keyStr).c_str()), std::string(*valueStr));

						}

						map->addGroup(group->field(), *group);

						delete group;

					}
				}
			}
		}
	}

	static void addFixTrailer(FIX::Message* message, Local<v8::Object> msg) {

		FIX::Trailer &msgTrailer = message->getTrailer();

		Local<v8::String> trailerKey = Nan::New<v8::String>("trailer").ToLocalChecked();

		if(msg->Has(trailerKey)) {

			Local<v8::Object> trailer = Local<v8::Object>::Cast(msg->Get(trailerKey));
			Local<v8::Array> trailerTags = trailer->GetPropertyNames();

			for(int i=0; i<(int)trailerTags->Length(); i++) {

				Local<v8::String> prop = trailerTags->Get(i)->ToString();
				String::Utf8Value keyStr(prop->ToString());

				String::Utf8Value valueStr(trailer->Get(prop)->ToString());

				msgTrailer.setField(atoi(std::string(*keyStr).c_str()), std::string(*valueStr));

			}

		}

	}

	static void js2Fix(FIX::Message* message, Local<v8::Object> msg) {

		addFixHeader(message, msg);

		addFixTags(message, msg);

		addFixGroups(message, msg);

		addFixTrailer(message, msg);

	}

	static void addJsHeader(Local<v8::Object> msg, const FIX::Message* message) {
		Local<v8::Object> header = Nan::New<v8::Object>();
		FIX::Header messageHeader = message->getHeader();

		for(FIX::FieldMap::iterator it = messageHeader.begin(); it != messageHeader.end(); ++it)
		{
			header->Set(Nan::New<Integer>(it->first), Nan::New<v8::String>(it->second.getString().c_str()).ToLocalChecked());
		}

		msg->Set(Nan::New<v8::String>("header").ToLocalChecked(), header);
	}

	static void addJsTags(Local<v8::Object> msg, const FIX::FieldMap* map) {
		Local<v8::Object> tags = Nan::New<v8::Object>();
		int noTags = 0;

		for(FIX::FieldMap::iterator it = map->begin(); it != map->end(); ++it)
		{
			tags->Set(Nan::New<Integer>(it->first), Nan::New<v8::String>(it->second.getString().c_str()).ToLocalChecked());
			noTags++;
		}

		if (noTags > 0) {
			msg->Set(Nan::New<v8::String>("tags").ToLocalChecked(), tags);
		}
	}

	static void addJsTrailer(Local<v8::Object> msg, const FIX::Message* message) {
		Local<v8::Object> trailer = Nan::New<v8::Object>();
		FIX::Trailer messageTrailer = message->getTrailer();

		for(FIX::FieldMap::iterator it = messageTrailer.begin(); it != messageTrailer.end(); ++it)
		{
			trailer->Set(Nan::New<Integer>(it->first), Nan::New<v8::String>(it->second.getString().c_str()).ToLocalChecked());
		}

		msg->Set(Nan::New<v8::String>("trailer").ToLocalChecked(), trailer);
	}

	static void addJsGroups(Local<v8::Object> msg, const FIX::FieldMap* map) {
		Local<v8::Object> groups = Nan::New<v8::Object>();
		int noGroups = 0;

		for(FIX::FieldMap::g_iterator it = map->g_begin(); it != map->g_end(); ++it) {
			std::vector< FIX::FieldMap* > groupVector = it->second;
			Handle<v8::Array> groupList = Nan::New<v8::Array>(groupVector.size());
			int i = 0;

			for(std::vector< FIX::FieldMap* >::iterator v_it = groupVector.begin(); v_it != groupVector.end(); ++v_it) {
				Local<v8::Object> groupEntry = Nan::New<v8::Object>();

				FIX::FieldMap* fields = *v_it;

				addJsTags(groupEntry, fields);

				addJsGroups(groupEntry, fields);

				groupList->Set(i, groupEntry);

				i++;
			}

			groups->Set(Nan::New<Integer>(it->first), groupList);

			noGroups++;
		}

		if (noGroups > 0) {
			msg->Set(Nan::New<v8::String>("groups").ToLocalChecked(), groups);
		}
	}

	static void fix2Js(Local<v8::Object> msg, const FIX::Message* message) {

		addJsHeader(msg, message);

		addJsTags(msg, message);

		addJsTrailer(msg, message);

		addJsGroups(msg, message);

	}

	static Local<Value> sessionIdToJs(const FIX::SessionID* sessionId) {
		Local<v8::Object> session = Nan::New<v8::Object>();

		session->Set(Nan::New<v8::String>("beginString").ToLocalChecked(), Nan::New<v8::String>(sessionId->getBeginString().getString().c_str()).ToLocalChecked());
		session->Set(Nan::New<v8::String>("senderCompID").ToLocalChecked(), Nan::New<v8::String>(sessionId->getSenderCompID().getString().c_str()).ToLocalChecked());
		session->Set(Nan::New<v8::String>("targetCompID").ToLocalChecked(), Nan::New<v8::String>(sessionId->getTargetCompID().getString().c_str()).ToLocalChecked());
		session->Set(Nan::New<v8::String>("sessionQualifier").ToLocalChecked(), Nan::New<v8::String>(sessionId->getSessionQualifier().c_str()).ToLocalChecked());

		return session;
	}

	static FIX::SessionID jsToSessionId(Local<v8::Object> sessionId) {
		String::Utf8Value beginString(sessionId->Get(Nan::New<v8::String>("beginString").ToLocalChecked())->ToString());
		String::Utf8Value senderCompId(sessionId->Get(Nan::New<v8::String>("senderCompID").ToLocalChecked())->ToString());
		String::Utf8Value targetCompId(sessionId->Get(Nan::New<v8::String>("targetCompID").ToLocalChecked())->ToString());
		return FIX::SessionID(std::string(*beginString),
				std::string(*senderCompId),
				std::string(*targetCompId));
	}
};

#endif /* FIXMESSAGEUTIL_H_ */
