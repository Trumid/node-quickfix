/*
 * FixLogon.h
 *
 *  Created on: Oct 1, 2014
 *      Author: kdeol
 */

#ifndef FIXLOGON_H_
#define FIXLOGON_H_
#include "quickfix/Message.h"
#include "quickfix/SessionID.h"
#include <v8.h>
#include <node.h>

typedef struct {
	const FIX::SessionID* sessionId;
	const FIX::Message* message;
	NanCallback* logon;
} fix_logon_t;

#endif /* FIXLOGON_H_ */
