{
  'targets': [
    {
      'target_name': 'node_quickfix',
      'sources': [ 'src/FixEvent.h', 'src/FixEventHandler.h', 'src/FixEventHandler.cpp', 'src/FixApplication.h', 'src/FixApplication.cpp', 'src/FixClient.h', 'src/FixClient.cpp', 'src/node_quickfix.cpp' ],
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
