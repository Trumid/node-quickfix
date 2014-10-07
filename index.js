// javascript shim that lets our object inherit from EventEmitter
var quickfix = require(__dirname + '/build/Release/node_quickfix.node');

var FIXInitiator = quickfix.FixInitiator;
var FIXAcceptor = quickfix.FixAcceptor;
exports.logonProvider = function(logon) {
	return new quickfix.FixLoginProvider(logon);
};

var events = require('events');

inherits(FIXInitiator, events.EventEmitter);
inherits(FIXAcceptor, events.EventEmitter);

exports.initiator = function(propertiesFile, options) {
	var initiator = new FIXInitiator(propertiesFile, {
		onCreate: function(sessionID) {
			initiator.emit('onCreate', { sessionID: sessionID });
		},
		onLogon: function(sessionID) {
			initiator.emit('onLogon', { sessionID: sessionID });
		},
		onLogout: function(sessionID) {
			initiator.emit('onLogout', { sessionID: sessionID });
		},
		toAdmin: function(sessionID) {
			initiator.emit('toAdmin', { message: message, sessionID: sessionID });
		},
		fromAdmin: function(message, sessionID) {
			initiator.emit('fromAdmin', { message: message, sessionID: sessionID });
		},
		toApp: function(message, sessionID) {
			initiator.emit('toApp', { message: message, sessionID: sessionID });
		},
		fromApp: function(message, sessionID) {
			initiator.emit('fromApp', { message: message, sessionID: sessionID });
		}
    }, options);

	return initiator;
};

exports.acceptor = function(propertiesFile, options) {
	var acceptor = new FIXAcceptor(propertiesFile, {
		onCreate: function(sessionID) {
			acceptor.emit('onCreate', { sessionID: sessionID });
		},
		onLogon: function(sessionID) {
			acceptor.emit('onLogon', { sessionID: sessionID });
		},
		onLogout: function(sessionID) {
			acceptor.emit('onLogout', { sessionID: sessionID });
		},
		toAdmin: function(message, sessionID) {
			acceptor.emit('toAdmin', { message: message, sessionID: sessionID });
		},
		fromAdmin: function(message, sessionID) {
			acceptor.emit('fromAdmin', { message: message, sessionID: sessionID });
		},
		toApp: function(message, sessionID) {
			acceptor.emit('toApp', { message: message, sessionID: sessionID });
		},
		fromApp: function(message, sessionID) {
			acceptor.emit('fromApp', { message: message, sessionID: sessionID });
		}
    }, options);

	return acceptor;
};

// extend prototype
function inherits(target, source) {
  for (var k in source.prototype)
    target.prototype[k] = source.prototype[k];
}