project('json-signer', 'c')

sodium_dep      = dependency('libsodium')      # ISC
json_c_dep      = dependency('json-c')         # MIT

cc = meson.get_compiler('c')
resolv_dep = cc.find_library('resolv', required: true)

executable(
    'json-signer', 
        'json-signer.c', 
    dependencies: [
        sodium_dep, 
        json_c_dep, 
        resolv_dep
    ],
    install: true
)
