{
  "targets": [
    {
      "target_name": "rklog_server",
      "include_dirs": [
        '../../include',
        "<!(node -e \"require('nan')\")"
      ],
      "libraries": [
        '../../../bin/librpc.a'
      ],
      "sources": [ "rklog_server_addon.cc", "rklog_server.cpp" ],
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions'],
      'xcode_settings': {
        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
        'CLANG_CXX_LIBRARY': 'libc++',
        'MACOSX_DEPLOYMENT_TARGET': '10.7',
      },
      'msvs_settings': {
        'VCCLCompilerTool': { 'ExceptionHandling': 1 },
      }
    }
  ]
}
