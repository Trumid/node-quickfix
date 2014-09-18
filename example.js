var df = require('dateformat')
var quickfix = require('./index');

var fixServer = quickfix.acceptor("./nodeQuickfixExample.properties");
fixServer.start(function() {
	console.log("FIX Acceptor Started")
});

var fixClient = quickfix.initiator("./nodeQuickfixExample.properties");

fixClient.start(function() {
	console.log("FIX Initiator Started")
});

var order = {
  header: {
    8: 'FIX.4.4',
    35: 'D',
    49: "TESTSUITE",
    56: "ELECGWTEST"
  },
  message: {
    11: "clordid",
    48: "cusip",
    22: 1,
    38: 1000,
    40: 2,
    54: 1,
    55: 'SYM',
    44: 99.75,
    60: df(new Date(), "yyyymmdd-HH:MM:ss.l"),
    423: 1
  }
};

fixClient.send(order, function() {
	console.log("Order sent!");
});




