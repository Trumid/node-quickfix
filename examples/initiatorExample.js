var df = require('dateformat');
var events = require('events');
var quickfix = require('../index');
var initiator = quickfix.initiator;

var options = {
  credentials: {
    username: "USERNAME",
    password: "PASSWORD"
  },
  propertiesFile: "./nodeQuickfixInitiatorExample.properties"
};


// extend prototype
function inherits (target, source) {
  for (var k in source.prototype)
    target.prototype[k] = source.prototype[k];
}

inherits(initiator, events.EventEmitter);

var fixClient = new initiator(
{
  onCreate: function(sessionID) {
    fixClient.emit('onCreate', { sessionID: sessionID });
  },
  onLogon: function(sessionID) {
    fixClient.emit('onLogon', { sessionID: sessionID });
  },
  onLogout: function(sessionID) {
    fixClient.emit('onLogout', { sessionID: sessionID });
  },
  onLogonAttempt: function(message, sessionID) {
    fixClient.emit('onLogonAttempt', { message: message, sessionID: sessionID });
  },
  toAdmin: function(message, sessionID) {
    fixClient.emit('toAdmin', { message: message, sessionID: sessionID });
  },
  fromAdmin: function(message, sessionID) {
    fixClient.emit('fromAdmin', { message: message, sessionID: sessionID });
  },
  fromApp: function(message, sessionID) {
    fixClient.emit('fromApp', { message: message, sessionID: sessionID });
  }
}, options);

fixClient.start(function() {
	console.log("FIX Initiator Started");
  var order = {
    header: {
      8: 'FIX.4.4',
      35: 'D',
      49: "NODEQUICKFIX",
      56: "ELECTRONIFIE"
    },
    tags: {
      11: "0E0Z86K00000",
      48: "06051GDX4",
      22: 1,
      38: 200,
      40: 2,
      54: 1,
      55: 'BAC',
      218: 100,
      60: df(new Date(), "yyyymmdd-HH:MM:ss.l"),
      423: 6
    }
  };

  fixClient.send(order, function() {
    console.log("Order sent!");
    process.stdin.resume();
  });
});

