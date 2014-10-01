{
  'targets': [
    {
      'target_name': 'node_quickfix',
      'sources': [
        'src/FixEvent.h',
        'src/FixSession.h',
        'src/FixSession.cpp',
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
        '/usr/local/include/quickfix'
      ],
      'cflags': [ '-fexceptions', '-std=c++11' ],
      'cflags_cc': [ '-fexceptions' ],
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
