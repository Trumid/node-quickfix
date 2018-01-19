var events = require('events');
var path = require('path');
var common = require('./common');
var quickfix = require('../index');
var fixAcceptor = quickfix.acceptor;

var logonProvider = new quickfix.logonProvider(function (logonResponse, msg, sessionId) {
  if (msg.tags[553] == 'USERNAME' && msg.tags[554] == 'PASSWORD') {
    logonResponse.done(true);
  } else {
    logonResponse.done(false);
  }
});

// extend prototype
function inherits(target, source) {
  for (var k in source.prototype)
    target.prototype[k] = source.prototype[k];
}

inherits(fixAcceptor, events.EventEmitter);

var fixServer = new fixAcceptor(
  {
    onCreate: function (sessionID) {
      fixServer.emit('onCreate', common.stats(fixServer, sessionID));
    },
    onLogon: function (sessionID) {
      fixServer.emit('onLogon', common.stats(fixServer, sessionID));
    },
    onLogout: function (sessionID) {
      fixServer.emit('onLogout', common.stats(fixServer, sessionID));
    },
    onLogonAttempt: function (message, sessionID) {
      fixServer.emit('onLogonAttempt', common.stats(fixServer, sessionID, message));
    },
    toAdmin: function (message, sessionID) {
      fixServer.emit('toAdmin', common.stats(fixServer, sessionID, message));
    },
    fromAdmin: function (message, sessionID) {
      fixServer.emit('fromAdmin', common.stats(fixServer, sessionID, message));
    },
    fromApp: function (message, sessionID) {
      fixServer.emit('fromApp', common.stats(fixServer, sessionID, message));
    }
  }, {
    logonProvider: logonProvider,
    propertiesFile: path.join(__dirname, 'acceptor.properties')
  });

['onCreate',
  'onLogon',
  'onLogout',
  'onLogonAttempt',
  'toAdmin',
  'fromAdmin',
  'fromApp']
  .forEach(function (event) {
    fixServer.on(event, console.log.bind(null, event));
  });

fixServer.start(function () {
  console.log("FIX Acceptor Started");
  common.printStats(fixServer);
  process.stdin.resume();
});