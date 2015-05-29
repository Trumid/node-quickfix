var path = require('path');
var quickfix = require('../');
var should = require('should');

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

  });

});