{
  "targets": [
    {
      "target_name": "rkdb_client",
      "include_dirs": [
        '../../include'
      ],
      "libraries": [
        '/home/byx/et/et_common/bin/librpc.a'
      ],
      "sources": [ "rkdb_client.cc" ],
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
