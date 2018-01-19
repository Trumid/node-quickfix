var NodeQuickfix = require('bindings')('NodeQuickfix')

exports.logonProvider = NodeQuickfix.FixLoginProvider;
exports.initiator = NodeQuickfix.FixInitiator;
exports.acceptor = NodeQuickfix.FixAcceptor;