var df = require('dateformat');
var events = require('events');
var quickfix = require('../index');
var common = require('./common');
var path = require('path');
var initiator = quickfix.initiator;

var options = {
  credentials: {
    username: "USERNAME",
    password: "PASSWORD"
  },
  propertiesFile: path.join(__dirname, "nodeQuickfixInitiatorExample.properties")
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
    fixClient.emit('onCreate', common.stats(fixClient, sessionID));
  },
  onLogon: function(sessionID) {
    fixClient.emit('onLogon', common.stats(fixClient, sessionID));
  },
  onLogout: function(sessionID) {
    fixClient.emit('onLogout', common.stats(fixClient, sessionID));
  },
  onLogonAttempt: function(message, sessionID) {
    fixClient.emit('onLogonAttempt', common.stats(fixClient, sessionID, message));
  },
  toAdmin: function(message, sessionID) {
    fixClient.emit('toAdmin', common.stats(fixClient, sessionID, message));
  },
  fromAdmin: function(message, sessionID) {
    fixClient.emit('fromAdmin', common.stats(fixClient, sessionID, message));
  },
  fromApp: function(message, sessionID) {
    fixClient.emit('fromApp', common.stats(fixClient, sessionID, message));
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
    common.printStats(fixClient);
    process.stdin.resume();
  });
});

