node-quickfix
==========

This is a node.js wrapper of the popular QuickFIX library. Information about QuickFIX can be found at http://quickfixengine.org/. FIX is a standardized messaging protocol used for electronic communication of financial information. More information about FIX can be found at http://www.fixprotocol.org/

###Installing######

1. Download quickfix tar here: http://www.quickfixengine.org/
2. Check required dependencies: http://www.quickfixengine.org/quickfix/doc/html/dependencies.html
  - On Ubuntu 14.04 LTS (Trusty Tahr) [zlib1g-dev](http://packages.ubuntu.com/trusty/zlib1g-dev) is required.
3. Install via the following instructions: http://www.quickfixengine.org/quickfix/doc/html/building.html
4. After installing copy "config.h" from the install directory to your include directory (usually /usr/local/include/quickfix)
5. Include this module in your package.json file.

###Features######

This module currently supports creating an Acceptor, an Initiator, and retrieving individual Sessions.
Examples can be found in the examples directory of this project.

####Configuration######

Sessions can be configured through a properties file or directly with a String (which can be built from a JSON variable). For example, if your properties file has the following:
```
[DEFAULT]
ReconnectInterval=60
SocketAcceptPort=3223
PersistMessages=Y
FileStorePath=./data
FileLogPath=./log
HttpAcceptPort=9011

[SESSION]
ConnectionType=acceptor
SenderCompID=ELECTRONIFIE
TargetCompID=NODEQUICKFIX
BeginString=FIX.4.4
StartTime=00:00:00
EndTime=23:59:59
HeartBtInt=30
UseDataDictionary=Y
DataDictionary=./fix_spec/FIX44.xml
RefreshOnLogon=Y
```
You can pass a String in to the options of your acceptor/initiator:
```
var quickfix = require('node-quickfix');

var fixAcceptor = quickfix.acceptor({
  settings: "[DEFAULT]\n
  	ReconnectInterval=60\n
  	RefreshOnLogon=Y\n
  	SendRedundantResendRequests=Y\n
  	PersistMessages=Y\n
  	FileStorePath=./data/1\n
  	FileLogPath=./log/1\n
  	HttpAcceptPort=9011\n\n
  	[SESSION]\n
  	ConnectionType=acceptor\n
  	SenderCompID=ELECTRONIFIE\n
  	TargetCompID=NODEQUICKFIX\n
  	BeginString=FIX.4.4\n
  	StartTime=00:00:00\n
  	EndTime=23:59:59\n
  	HeartBtInt=30\n
  	SocketAcceptPort=3223\n
  	UseDataDictionary=Y\n
  	DataDictionary=./fix_spec/FIX44.xml",
  logonProvider: logonProvider,
  storeFactory: "file"
});
```

node-quickfix also has database support for quickfix. Provided you have the correct database installed, and have compiled quickfix with database support as described in http://www.quickfixengine.org/quickfix/doc/html/building.html , then using the database is as simple as updating your properties and then creating an acceptor with the correct storeFactory (file, postgresql, mysql, odbc). node-quickfix will use file storage by default.

```
[DEFAULT]
ReconnectInterval=60
SocketAcceptPort=3223
PersistMessages=Y
PostgreSQLStoreDatabase=quickfix
PostgreSQLStoreUser=postgres
PostgreSQLStorePassword=postgres
PostgreSQLStoreHost=localhost
PostgreSQLStorePort=5432
PostgreSQLStoreUseConnectionPool=Y
PostgreSQLLogDatabase=quickfix
PostgreSQLLogUser=postgres
PostgreSQLLogPassword=postgres
PostgreSQLLogHost=localhost
PostgreSQLLogPort=5432
PostgreSQLLogUseConnectionPool=Y
HttpAcceptPort=9011
```

```
var fixAcceptor = quickfix.acceptor({
  propertiesFile: './acceptor.properties',
  logonProvider: logonProvider,
  storeFactory: 'postgresql'
});
```

####Logon Handling#######

You can create a custom logon handler in node-quickfix by creating a logon provider and then constructing your acceptor or initiator with it. You can also pass credentials to the constructor that will be used to populate field 553 and 554 of your logon messages.

```
var quickfix = require('node-quickfix');

var logonProvider = quickfix.logonProvider(function(logonResponse, msg, sessionId) {
	if(msg.tags[553] == 'USERNAME' && msg.tags[554] == 'PASSWORD') {
		logonResponse.done(true); //successful logon
	} else {
		logonResponse.done(false); //reject logon
	}
});

var fixAcceptor = quickfix.acceptor({
  "logonProvider": logonProvider,
  propertiesFile: "./acceptor.properties"
});
```

```
var fixClient = quickfix.initiator({
  credentials: {
    username: "USERNAME",
    password: "PASSWORD"
  },
  propertiesFile: "./initiator.properties"
});
```

####Message format######
```
message = {
    header: {
      8: 'FIX.4.4',
      35: type,
      49: senderId,
      56: targetId
    },
    tags: {
        tag1: value1,
        tag2: value2
    },
    groups:[{
        'index': field for NoOfEntries,
        'delim': first tag in group,
        'entries': [{tag1: value1, tag2: value2}, {tag1: value1, tag2: value2}]
    }, ...]
  }
```

####Acceptor API######
`start(callback) //start acceptor`

`send(msg, callback) //send json messages`

`sendRaw(msg, callback) //send FIX message string`

`stop(callback) //stop acceptor`

`getSessions() //get configured sessions`

`getSession(sessionId) //get session by session ID`


Acceptors and initiators inherit from EventEmitter and so the following events can be listened for on acceptors/initiators with `fixAcceptor.on(eventName, callback);` Valid eventNames and the callback arguments are:
```
onCreate: sessionID
onLogon: sessionID
onLogout: sessionID
onLogonAttempt:  message, sessionID
toAdmin: message, sessionID
fromAdmin: message, sessionID
fromApp:  message, sessionID
```

####Initiator API######
`start(callback) //start acceptor`

`send(msg, callback) //send json messages`

`sendRaw(msg, callback) //send FIX message string`

`stop(callback) //stop acceptor`

`getSessions() //get configured sessions`

`getSession(sessionId) //get session by session ID`

`isLoggedOn() //is initiator is currently connected to a session`


####Session API######
`disconnect(callback) //disconnect a session`

`getSessionID() //get session id for a session`

`isEnabled() //is the session enabled`

`isLoggedOn() //is the session currently logged on`

`logon(callback) //log the session on`

`logout(callback) //log the session out`

`refresh(callback) //refresh session state from the message store`

`reset(callback) //reset session sequence numbers`

getters/setters for session sequence numbers:

```
getSenderSeqNum()
setSenderSeqNum(num)
getTargetSeqNum()
setSenderSeqNum(num)
```

