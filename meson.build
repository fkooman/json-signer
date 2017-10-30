project('json-signer', 'c')
sodiumdep = dependency('libsodium')
glibdep = dependency('glib-2.0')
jsoncdep = dependency('json-c')
bsddep = dependency('libbsd')

executable('json-signer', 'json-signer.c', dependencies : [sodiumdep, glibdep, jsoncdep, bsddep])
