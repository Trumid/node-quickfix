var events = require('events');
var path = require('path');
var quickfix = require('../index');
var fixAcceptor = quickfix.acceptor;

var logonProvider = new quickfix.logonProvider(function (logonResponse, msg, sessionId) {	
	if(msg.tags[553] == 'USERNAME' && msg.tags[554] == 'PASSWORD') {
		logonResponse.done(true);
	} else {
		logonResponse.done(false);
	}
});

// extend prototype
function inherits (target, source) {
  for (var k in source.prototype)
    target.prototype[k] = source.prototype[k];
}

inherits(fixAcceptor, events.EventEmitter);

var fixServer = new fixAcceptor(
{
  onCreate: function(sessionID) {
    fixServer.emit('onCreate', { sessionID: sessionID });
  },
  onLogon: function(sessionID) {
    fixServer.emit('onLogon', { sessionID: sessionID });
  },
  onLogout: function(sessionID) {
    fixServer.emit('onLogout', { sessionID: sessionID });
  },
  onLogonAttempt: function(message, sessionID) {
    fixServer.emit('onLogonAttempt', { message: message, sessionID: sessionID });
  },
  toAdmin: function(message, sessionID) {
    fixServer.emit('toAdmin', { message: message, sessionID: sessionID });
  },
  fromAdmin: function(message, sessionID) {
    fixServer.emit('fromAdmin', { message: message, sessionID: sessionID });
  },
  fromApp: function(message, sessionID) {
    fixServer.emit('fromApp', { message: message, sessionID: sessionID });
  }
}, {
  logonProvider: logonProvider, 
  propertiesFile: path.join(__dirname, 'nodeQuickfixExample.properties')
});

['onCreate',
'onLogon',
'onLogout',
'onLogonAttempt',
'toAdmin',
'fromAdmin',
'fromApp'].forEach(function (event) {
  fixServer.on(event, console.log.bind(null, event));
});

fixServer.start(function() {
	console.log("FIX Acceptor Started");
  process.stdin.resume();
});