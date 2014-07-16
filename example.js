var quickfix = require('./build/Release/node_quickfix');
var SegfaultHandler = require('segfault-handler');
var df = require('dateformat');

SegfaultHandler.registerHandler();

var outsideVar = "Hey am I out of scope?";

var createTagField = function(tag, object) {
	return {key: tag, value: object};
}

var client = quickfix.Initiator({
    'onLogon': function() {},
    'onLogout': function() {},
	'toApp': function() {},
	'fromApp': function(message, sessionId) {
		console.log("In javascript fromApp...we made it back!");
		console.log(JSON.stringify(sessionId, null, 4) + ": " + JSON.stringify(message, null, 4));
		console.log(outsideVar);
	}
});

client.start('nodeQuickfixExample.properties', function() {
	console.log("Server started.");
	var msg = {
		header:[
			createTagField(35, 'D')
		],
		message:[
			createTagField(11, 'ClOrdId'),
	        createTagField(48, '00206RAG'),
	        createTagField(22, 1),
	        createTagField(38, 1000),
	        createTagField(40, 2),
	        createTagField(54, 1),
	        createTagField(55, 'N/A'),
	        createTagField(44, 99.75),
	        //add this dateformat function to the node lib probably
	        createTagField(60, df(new Date(), "yyyymmdd-HH:MM:ss.l")),
	        createTagField(423, 1)
		]
	};
	
	client.send(msg, function(){
		console.log("ZOMG MESSAGE SENT!");
	});
	
	client.send(msg, function(){
		console.log("ZOMG MESSAGE SENT AGAIN!");
		//client.stop();
	});
});


