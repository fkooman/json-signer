# Introduction

**EXPERIMENTAL** reimplementation of 
[php-json-signer](https://github.com/fkooman/php-json-signer) in C.

**DO NOT USE**

# Requirements

Install the required dependencies.

## Fedora
    
    $ sudo dnf install glib2-devel json-c-devel libsodium-devel meson git gcc

## CentOS
    
Enable EPEL repository first.

    $ sudo yum install glib2-devel json-c-devel libsodium-devel meson git gcc

## Debian

    $ sudo apt install build-essential meson libsodium-dev libjson-c-dev libglib2.0-dev

# Building

    $ git clone https://github.com/fkooman/json-signer.git
    $ cd json-signer
    $ meson build
    $ (cd build && ninja-build)

# Usage

    $ build/json-signer file.json

# TODO

- create the XDG data directory on first use, now required to do that manually;
- actually test it;
- no longer depend on `glib-2.0`
  - need to find good libraries for base64 and XDG;
- make sure it doesn't leak memory;
- static code analysis;
