var events = require('events');
var path = require('path');
var quickfix = require('../');
var should = require('should');

var acceptorPropertiesFile = path.resolve(path.join(__dirname, 'support', 'acceptor.properties'));
var initiatorPropertiesFile = path.resolve(path.join(__dirname, 'support', 'initiator.properties'));

describe('node-quickfix', function () {

  describe('initiator', function () {
  
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
      initiator.getSessions()[0].senderCompID.should.eql('NODEQUICKFIX');
      initiator.getSessions()[0].targetCompID.should.eql('ELECTRONIFIE');
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
      acceptor.getSessions()[0].senderCompID.should.eql('ELECTRONIFIE');
      acceptor.getSessions()[0].targetCompID.should.eql('NODEQUICKFIX');
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

  describe('inititator + acceptor', function () {

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
  });

});