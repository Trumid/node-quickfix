var df = require('dateformat')
var quickfix = require('../index');

var logonProvider = quickfix.logonProvider(function(logonResponse, msg, sessionId) {
	console.log("In JS Logon method");
	if(msg.tags[553] == 'USERNAME' && msg.tags[554] == 'PASSWORD') {
		logonResponse.done(true);
	} else {
		logonResponse.done(false);
	}
});

var fixServer = quickfix.acceptor({"logonProvider": logonProvider, propertiesFile: "./nodeQuickfixExample.properties"});

fixServer.start(function() {
	console.log("FIX Acceptor Started")
});