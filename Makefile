REPORTER=spec

acceptor:
	node ./examples/acceptorExample.js

initiator:
	node ./examples/initiatorExample.js

test:
	DEBUG= ./node_modules/.bin/mocha -R $(REPORTER) test

test-jenkins:
	$(MAKE) JUNIT_REPORT_PATH=test-report.xml JUNIT_REPORT_STACK=1 REPORTER=mocha-jenkins-reporter test

.PHONY: test
