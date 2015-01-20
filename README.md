node-quickfix
==========

This is a node.js wrapper of the popular QuickFIX library. Information about QuickFIX can be found at http://quickfixengine.org/. FIX is a standardized messaging protocol used for electronic communication of financial information. More information about FIX can be found at http://www.fixprotocol.org/

###Installing######

1. Download quickfix tar here: http://www.quickfixengine.org/
2. Install via the following instructions: http://www.quickfixengine.org/quickfix/doc/html/building.html
3. After installing copy "config.h" from the install directory to your include directory (usually /usr/local/include/quickfix)
3. Install the TBB library from https://www.threadingbuildingblocks.org/download (brew install tbb on OSX or yum install tbb tbb-devel)
4. Include this module in your package.json file.

###Features######

This module currently supports creating an Acceptor, an Initiator, and retrieving individual Sessions.
Examples can be found in the examples directory of this project. 