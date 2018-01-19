REPORTER=spec

acceptor:
	node ./examples/acceptor.js

initiator:
	node ./examples/initiator.js

test:
	DEBUG= ./node_modules/.bin/mocha -R $(REPORTER) test

.PHONY: test
