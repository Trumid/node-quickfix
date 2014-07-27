{
  'targets': [
    {
      'target_name': 'node_quickfix',
      'sources': [ 
      	'src/FixEvent.h', 
      	'src/FixApplication.h', 
      	'src/FixApplication.cpp', 
      	'src/FixInitiator.h', 
      	'src/FixInitiator.cpp', 
      	'src/FixStartWorker.h', 
      	'src/FixStartWorker.cpp',
      	'src/FixSendWorker.h', 
      	'src/FixSendWorker.cpp',
      	'src/FixStopWorker.h', 
      	'src/FixStopWorker.cpp',
      	'src/node_quickfix.cpp' ],
	  'link_settings': {
	  	'libraries': [
	  		'-L/usr/lib',
	  		'-L/usr/local/lib',
	  		'-lquickfix',
	  		'-lpthread', 
	  		'-lxml2',
	  		'-lz'
	  	]
	  },
	  'include_dirs': [
	  	"<!(node -e \"require('nan')\")",
	  	'/usr/local/include/quickfix'
	  ],
	  'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],
      'conditions': [
        ['OS=="mac"', {
          'xcode_settings': {
            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
            "OTHER_CFLAGS": ["-mmacosx-version-min=10.7", "-stdlib=libc++"]
          }
        }]
      ]
    }
  ]
}
