project('json-signer', 'c')
sodium_dep = dependency('libsodium')
jsonc_dep = dependency('json-c')
xdg_dep = dependency('libxdg-basedir')

cc = meson.get_compiler('c')
m_dep = cc.find_library('resolv', required : true)

executable('json-signer', 'json-signer.c', install: true, dependencies : [sodium_dep, jsonc_dep, xdg_dep, m_dep])
