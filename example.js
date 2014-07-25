var quickfix = require('./build/Release/node_quickfix');
var SegfaultHandler = require('segfault-handler');
var df = require('dateformat');

SegfaultHandler.registerHandler();

var responseCount = 0;

var createTagField = function(tag, object) {
	return {key: tag, value: object};
}

var client = quickfix.Initiator({
    'onLogon': function(sessionId) {},
    'onLogout': function(sessionId) {},
	'toApp': function(message, sessionId) {},
	'fromApp': function(message, sessionId) {
		console.log("In javascript fromApp...we made it back!");
		console.log(JSON.stringify(sessionId, null, 4) + ": " + JSON.stringify(message, null, 4));
		responseCount++;
		if(responseCount == 2) {
			//both messages received responses so we can shut down our initiator now
			client.stop();
		}
	}
});

client.start('nodeQuickfixExample.properties', function() {
	console.log("Server started.");
	var msg = {
		header:{
			35: 'D'
		},
		message:{
			11: 'ClOrdId',
	        48: '01748XAA0',
	        22: 1,
	        38: 1000,
	        40: 2,
	        54: 1,
	        55: 'N/A',
	        44: 99.75,
	        //add this dateformat function to the node lib probably
	        60: df(new Date(), "yyyymmdd-HH:MM:ss.l"),
	        423: 1
		}
	};
	
	client.send(msg, function(){
		console.log("ZOMG MESSAGE SENT!");
	});
	
	client.send(msg, function(){
		console.log("ZOMG MESSAGE SENT AGAIN!");
	});
});


