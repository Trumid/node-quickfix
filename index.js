var quickfix = require(__dirname + '/build/Release/node_quickfix.node');

var FIXInitiator = quickfix.FixInitiator;
var FIXAcceptor = quickfix.FixAcceptor;

exports.logonProvider = function(logon) {
	return new quickfix.FixLoginProvider(logon);
};

exports.initiator = function (callbacks, options) {
	var initiator = new FIXInitiator(callbacks, options);

	return initiator;
};

exports.acceptor = function (callbacks, options) {
	var acceptor = new FIXAcceptor(callbacks, options);

	return acceptor;
};
