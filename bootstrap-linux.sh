#!/bin/bash

set -e

BUILD_LINUX=1
BUILD_SNAP=1
PREFIX_ARG="-DCMAKE_INSTALL_PREFIX=$CRAFT_PART_INSTALL/usr"

if [ "$BUILD_LINUX" = "1" ]; then
    mkdir build-linux || true
    cd build-linux

    SRCDIR="../llvm"

    env CC=/usr/bin/clang CXX=/usr/bin/clang++ cmake -GNinja \
        -DLLVM_TARGETS_TO_BUILD="AArch64;X86;WebAssembly" \
        -DLLVM_ENABLE_PROJECTS='clang;lld;lldb' \
        -DLLVM_ENABLE_EH=ON \
        -DLLVM_ENABLE_RTTI=ON \
        -DLLVM_LINK_LLVM_DYLIB=ON \
        -DLLDB_INCLUDE_TESTS=OFF \
        -DCMAKE_BUILD_TYPE=Release \
        $PREFIX_ARG \
        $SRCDIR
    ninja
    ninja install
fi

exit 0
