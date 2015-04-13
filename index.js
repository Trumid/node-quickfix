var quickfix = require(__dirname + '/build/Release/node_quickfix.node');

var FIXInitiator = quickfix.FixInitiator;
var FIXAcceptor = quickfix.FixAcceptor;

exports.logonProvider = quickfix.FixLoginProvider;

exports.initiator = FIXInitiator;

exports.acceptor = FIXAcceptor;
