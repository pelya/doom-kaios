#!/bin/bash

[ -z "`which emsdk`" ] && export PATH=`pwd`/../emsdk:$PATH

[ -z "`which emsdk`" ] && { echo "Put emsdk into your PATH"; exit 1 ; }

[ -z "$PATH_EMSDK" ] && PATH_EMSDK="`which emsdk | xargs dirname`"

source "$PATH_EMSDK/emsdk_env.sh"

mkdir -p release
cd release

[ -e Makefile ] || emcmake cmake -DCMAKE_BUILD_TYPE=Release .. || exit 1

emmake make -j8 || exit 1

cd ..
./package-app.sh release
./install.sh release
