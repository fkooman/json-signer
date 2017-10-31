# Introduction

**EXPERIMENTAL** reimplementation of 
[php-json-signer](https://github.com/fkooman/php-json-signer) in C.

**DO NOT USE**

# Requirements

Install the required dependencies.

## Fedora
    
    $ sudo dnf install json-c-devel libsodium-devel libxdg-basedir-devel \
        meson git gcc

## CentOS
    
Enable EPEL repository first. It seems libxdg-basedir-devel is NOT available
on CentOS :(

    $ sudo yum install json-c-devel libsodium-devel libxdg-basedir-devel \
        meson git gcc

## Debian

    $ sudo apt install libjson-c-dev libsodium-dev libxdg-basedir-dev \
        build-essential meson git

# Building

    $ git clone https://github.com/fkooman/json-signer.git
    $ cd json-signer
    $ meson build
    $ (cd build && ninja-build)

You can also use [Clang](https://clang.llvm.org/), for that use:

    $ CC=clang && meson build

# Usage

    $ build/json-signer file.json

# TODO

- create the XDG data directory on first use, now required to do that manually;
- make sure it doesn't leak memory;
- static code analysis;
