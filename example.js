var df = require('dateformat')
var quickfix = require('./index');

var logonProvider = quickfix.logonProvider(function(msg, sessionId) {
	console.log("In JS Logon method");
	logonProvider.finish(true);
});

var fixServer = quickfix.acceptor("./nodeQuickfixExample.properties", logonProvider);

fixServer.start(function() {
	console.log("FIX Acceptor Started")
});