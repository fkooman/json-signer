project('json-signer', 'c')

sodium_dep      = dependency('libsodium')
json_c_dep      = dependency('json-c')
xdg_basedir_dep = dependency('libxdg-basedir')

cc = meson.get_compiler('c')
resolv_dep = cc.find_library('resolv', required: true)

executable(
    'json-signer', 
        'json-signer.c', 
    install: true, 
    dependencies: [
        sodium_dep, 
        json_c_dep, 
        xdg_basedir_dep, 
        resolv_dep
    ]
)
