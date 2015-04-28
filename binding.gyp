{
  'targets': [
    {
      'target_name': 'node_quickfix',
      'product_extension': 'node',
      'type': 'shared_library',
      'sources': [
        'src/Threading.h',
        'src/Dispatcher.h',
      	'src/FixCredentials.h',
      	'src/FixEvent.h',
      	'src/FixEventQueue.h',
      	'src/FixSession.h',
      	'src/FixSession.cpp',
      	'src/FixLogon.h',
      	'src/FixLoginProvider.h',
      	'src/FixLoginProvider.cpp',
      	'src/FixLoginResponse.h',
      	'src/FixLoginResponse.cpp',
      	'src/FixAcceptor.h',
      	'src/FixAcceptor.cpp',
      	'src/FixAcceptorStartWorker.h',
      	'src/FixAcceptorStartWorker.cpp',
      	'src/FixAcceptorStopWorker.h',
      	'src/FixAcceptorStopWorker.cpp',
      	'src/FixApplication.h',
      	'src/FixApplication.cpp',
      	'src/FixConnection.h',
      	'src/FixConnection.cpp',
      	'src/FixInitiator.h',
      	'src/FixInitiator.cpp',
      	'src/FixInitiatorStartWorker.h',
      	'src/FixInitiatorStartWorker.cpp',
      	'src/FixInitiatorStopWorker.h',
      	'src/FixInitiatorStopWorker.cpp',
      	'src/FixMessageUtil.h',
      	'src/FixSendWorker.h',
      	'src/FixSendWorker.cpp',
      	'src/node_quickfix.cpp'
      ],
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
        '/usr/local/include',
        '/usr/local/include/quickfix'
      ],
      'direct_dependent_settings': {
        'include_dirs': ['src']
      },
      'cflags': [ '-fexceptions', '-std=c++11' ],
      'cflags!': ['-fno-exceptions', '-fno-rtti'],
      'cflags_cc': [ '-fexceptions' ],
      'cflags_cc!': [ '-fno-exceptions', '-fno-rtti' ],
      'conditions': [
        ['OS=="mac"', {
          'xcode_settings': {
            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
            'GCC_ENABLE_CPP_RTTI': 'YES',
            "OTHER_CFLAGS": ["-mmacosx-version-min=10.7", "-stdlib=libc++"]
          }
        }]
      ]
    }
  ]
}
