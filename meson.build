project('json-signer', 'c')
sodiumdep = dependency('libsodium')
jsoncdep = dependency('json-c')
xdgdep = dependency('libxdg-basedir')

executable('json-signer', 'json-signer.c', 'base64.c', dependencies : [sodiumdep, jsoncdep, xdgdep])
