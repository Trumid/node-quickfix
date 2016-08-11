var events = require('events');
var quickfix = require('../');
var path = require('path');
var should = require('should');

var segfaultHandler = require('segfault-handler');
segfaultHandler.registerHandler();

var acceptorPropertiesFile = path.resolve(path.join(__dirname, 'support', 'acceptor.properties'));
var initiatorPropertiesFile = path.resolve(path.join(__dirname, 'support', 'initiator.properties'));

describe('initiator', function () {

 this.timeout(5000);

 it('should throw if not supplied options', function () {
   (function () { quickfix.initiator(); }).should.throw('FixInitiator requires an options parameter');
 });

 it('should throw if supplied options with no settings or propertiesFile', function () {
   (function () { quickfix.initiator({}, {}); }).should.throw('you must provide FixInitiator either an options.settings string or options.propertiesFile path to a properties file');
 });

 it('should create a new initiator instance with fix beginString, senderCompID, targetCompID on defined session', function () {
   var initiator = new quickfix.initiator({}, {
     propertiesFile: initiatorPropertiesFile
    });
   initiator.should.not.eql(null);
   initiator.isLoggedOn().should.eql(false);
   initiator.getSessions()[0].beginString.should.eql('FIX.4.4');
   initiator.getSessions()[0].senderCompID.should.eql('SenderCompID');
   initiator.getSessions()[0].targetCompID.should.eql('TargetCompID');
 });

 it('should start and stop when called', function (done) {
   var initiator = new quickfix.initiator({}, {
     propertiesFile: initiatorPropertiesFile
   });
   initiator.start(function () {
     // todo: started with local property
     initiator.stop(function () {
        // todo: started with local property
       done();
      });
    });
 });

});

describe('acceptor', function () {

 this.timeout(5000);

 it('should throw if not supplied options', function () {
   (function () { quickfix.acceptor(); }).should.throw('FixAcceptor requires an options parameter');
  });

 it('should throw if supplied options with no settings or propertiesFile', function () {
   (function () { quickfix.acceptor({}, {}); }).should.throw('you must provide FixAcceptor either an options.settings string or options.propertiesFile path to a properties file');
 });

 it('should create a new acceptor instance with fix beginString, senderCompID, targetCompID on defined session', function () {
   var acceptor = new quickfix.acceptor({}, {
     propertiesFile: acceptorPropertiesFile
    });
   acceptor.should.not.eql(null);
   acceptor.getSessions()[0].beginString.should.eql('FIX.4.4');
   acceptor.getSessions()[0].senderCompID.should.eql('TargetCompID');
   acceptor.getSessions()[0].targetCompID.should.eql('SenderCompID');
 });

 it('should start and stop when called', function (done) {
   var acceptor = new quickfix.acceptor({}, {
     propertiesFile: acceptorPropertiesFile
    });
   acceptor.start(function () {
     // todo: started with local property
     acceptor.stop(function () {
        // todo: started with local property
       done();
      });
    });
  });
});

/*describe('initiator + acceptor', function () {

 this.timeout(5000);

 var acceptorEmitter = new events.EventEmitter();
 var initiatorEmitter = new events.EventEmitter();

 var acceptor = new quickfix.acceptor({
   onCreate: acceptorEmitter.emit.bind(acceptorEmitter, 'onCreate'),
   onLogon: acceptorEmitter.emit.bind(acceptorEmitter, 'onLogon'),
   onLogout: acceptorEmitter.emit.bind(acceptorEmitter, 'onLogout'),
   onLogonAttempt: acceptorEmitter.emit.bind(acceptorEmitter, 'onLogonAttempt'),
   toAdmin: acceptorEmitter.emit.bind(acceptorEmitter, 'toAdmin'),
   fromAdmin: acceptorEmitter.emit.bind(acceptorEmitter, 'fromAdmin'),
   toApp: acceptorEmitter.emit.bind(acceptorEmitter, 'toApp'),
   fromApp: acceptorEmitter.emit.bind(acceptorEmitter, 'fromApp'),
 }, {
   propertiesFile: acceptorPropertiesFile
 });

 var initiator = new quickfix.initiator({
   onCreate: initiatorEmitter.emit.bind(initiatorEmitter, 'onCreate'),
   onLogon: initiatorEmitter.emit.bind(initiatorEmitter, 'onLogon'),
   onLogout: initiatorEmitter.emit.bind(initiatorEmitter, 'onLogout'),
   onLogonAttempt: initiatorEmitter.emit.bind(initiatorEmitter, 'onLogonAttempt'),
   toAdmin: initiatorEmitter.emit.bind(initiatorEmitter, 'toAdmin'),
   fromAdmin: initiatorEmitter.emit.bind(initiatorEmitter, 'fromAdmin'),
   toApp: initiatorEmitter.emit.bind(initiatorEmitter, 'toApp'),
   fromApp: initiatorEmitter.emit.bind(initiatorEmitter, 'fromApp'),
 }, {
   propertiesFile: initiatorPropertiesFile
 });

 var results = {};

 function setResult (result) {
   results[result] = true;
 }

 [ 'onCreate',
   'onLogon',
   'onLogout',
   'onLogonAttempt',
   'toAdmin',
   'fromAdmin',
   'toApp',
   'fromApp'].forEach(function (e) {
   acceptorEmitter.on(e, setResult.bind(null, 'acceptor.' + e));
   initiatorEmitter.on(e, setResult.bind(null, 'initiator.' + e));
 });

 after(function (done) {
   initiator.stop(function () {
     acceptor.stop(done);
    });
 });

 before(function (done) {
   acceptor.start(function () {
     initiator.start(done);
    });
 });

 it('should logon and negotiate connection', function (done) {
   // TODO: come up with event to hook off off for this, instead of timeout
   setTimeout(function () {
     results['initiator.toAdmin'].should.eql(true);
     results['acceptor.fromAdmin'].should.eql(true);
     results['acceptor.toAdmin'].should.eql(true);
     results['acceptor.onLogon'].should.eql(true);
     results['initiator.fromAdmin'].should.eql(true);
     results['initiator.onLogon'].should.eql(true);
     done();
   }, 1000);
 });

 it('should convert nested groups from json to fix and back', function (done) {
   var message = {
     header: {
       8: 'FIXT.1.1',
       35: 'CM',
       49: 'SenderCompID2',
       56: 'TargetCompID2'
     },
     tags: {
       1511: 0,
       1512: 2,
       1666: '6ca92854-a64c-453c-9d8f-cfd2c7d643d9',
       1667: '6ca92854-a64c-453c-9d8f-cfd2c7d643d9',
       1760: 1
     },
     groups: [
       {
         index: 1677,
         delim: 1671,
         entries: [{
           groups: [{
             index: 1671,
             delim: 1691,
             entries: [{
               tags: {
                 1691: '650000089X',
                 1692: 'D',
                 1693: 3
               }
             }]
           }],
         }, {
           groups: [{
             index: 1671,
             delim: 1691,
             entries: [{
               tags: {
                 1691: '660000022X',
                 1692: 'D',
                 1693: 3
               }
             }]
           }]
         }]
       }
     ]
   };

   acceptorEmitter.once('fromApp', function (message) {
     message.header['8'].should.equal('FIXT.1.1');
     message.header['35'].should.equal('CM');
     message.header['49'].should.equal('SenderCompID2');
     message.header['56'].should.equal('TargetCompID2');
     message.tags['1511'].should.equal('0');
     message.tags['1512'].should.equal('2');
     message.tags['1666'].should.equal('6ca92854-a64c-453c-9d8f-cfd2c7d643d9');
     message.tags['1667'].should.equal('6ca92854-a64c-453c-9d8f-cfd2c7d643d9');
     message.tags['1677'].should.equal('2');
     message.tags['1760'].should.equal('1');
     message.groups['1677'].should.have.property('length', 2);
     message.groups['1677'][0].tags.should.have.property('1671', '1');
     message.groups['1677'][0].groups['1671'].should.have.property('length', 1);
     message.groups['1677'][0].groups['1671'][0].tags.should.have.property('1691', '650000089X');
     message.groups['1677'][0].groups['1671'][0].tags.should.have.property('1692', 'D');
     message.groups['1677'][0].groups['1671'][0].tags.should.have.property('1693', '3');
     message.groups['1677'][1].tags.should.have.property('1671', '1');
     message.groups['1677'][1].groups['1671'].should.have.property('length', 1);
     message.groups['1677'][1].groups['1671'][0].tags.should.have.property('1691', '660000022X');
     message.groups['1677'][1].groups['1671'][0].tags.should.have.property('1692', 'D');
     message.groups['1677'][1].groups['1671'][0].tags.should.have.property('1693', '3');
     done();
    });

   initiator.send(message);

  });

 it('should convert non-nested groups from json to fix and back', function (done) {

   var message = {
     header: {
       8: 'FIXT.1.1',
       35: 'CM',
       49: 'SenderCompID2',
       56: 'TargetCompID2'
     },
     tags: {
       1511: 0,
       1512: 2,
       1666: '6ca92854-a64c-453c-9d8f-cfd2c7d643d9',
       1667: '6ca92854-a64c-453c-9d8f-cfd2c7d643d9',
       1760: 1
     },
     groups: [
       {
         'index': 1677,
         'delim': 1671,
         'entries': [{
             1671: 1,
             1691: '650000089X',
             1692: 'D',
             1693: 3,
             // 1669: 1,
             // 1529: 1,
             // 1530: 1,
             // 1531: 200000000,
             // 1766: 15000000,
             // 1532: 'USD',
             // 1670: 123,
             // 2339: 3
         }, {
           1671: 1,
           1691: '660000022X',
           1692: 'D',
           1693: 3,
           // 1669: 1,
           // 1529: 1,
           // 1530: 1,
           // 1531: 150000000,
           // 1766: 5000000,
           // 1532: 'USD',
           // 1670: 456,
           // 2339: 3
         }]
       }
     ]
   };

   acceptorEmitter.once('fromApp', function (message) {
     message.header['8'].should.equal('FIXT.1.1');
     // message.header['9'].should.equal('256');
     message.header['35'].should.equal('CM');
     message.header['49'].should.equal('SenderCompID2');
     message.header['56'].should.equal('TargetCompID2');
     message.tags['1511'].should.equal('0');
     message.tags['1512'].should.equal('2');
     message.tags['1666'].should.equal('6ca92854-a64c-453c-9d8f-cfd2c7d643d9');
     message.tags['1667'].should.equal('6ca92854-a64c-453c-9d8f-cfd2c7d643d9');
     message.tags['1677'].should.equal('2');
     message.tags['1760'].should.equal('1');
     done();
   });

   initiator.send(message);

 });

});*/