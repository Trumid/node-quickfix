var df = require('dateformat')
var quickfix = require('../index');

var logonProvider = quickfix.logonProvider(function(msg, sessionId) {
	console.log("In JS Logon method");
	if(msg.tags[553] == 'USERNAME' && msg.tags[554] == 'PASSWORD') {
		logonProvider.finish(true);
	} else {
		logonProvider.finish(false);
	}
});

var fixServer = quickfix.acceptor("./nodeQuickfixExample.properties", {"logonProvider": logonProvider});

fixServer.start(function() {
	console.log("FIX Acceptor Started")
});