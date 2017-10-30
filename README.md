# Introduction

**EXPERIMENTAL** reimplementation of 
[php-json-signer](https://github.com/fkooman/php-json-signer).

**DO NOT USE**

# Usage

    $ git clone https://github.com/fkooman/json-signer.git
    $ cd json-signer
    $ meson build
    $ build/json-signer file.json
    
# TODO

- actually test it;
- no longer depend on `glib-2.0`
  - need to find good libraries for base64 and XDG;
- make sure it doesn't leak memory;
- static code analysis;
