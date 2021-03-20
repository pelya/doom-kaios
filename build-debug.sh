#!/bin/bash

[ -z "`which emsdk`" ] && export PATH=`pwd`/../emsdk:$PATH

[ -z "`which emsdk`" ] && { echo "Put emsdk into your PATH"; exit 1 ; }

[ -z "$PATH_EMSDK" ] && PATH_EMSDK="`which emsdk | xargs dirname`"

source "$PATH_EMSDK/emsdk_env.sh"

mkdir -p debug
cd debug

[ -e Makefile ] || emcmake cmake -DCMAKE_BUILD_TYPE=Debug .. || exit 1

rm -f *.html

emmake make -j8 VERBOSE=1 || exit 1

cd ..
./package-app.sh debug
./install.sh debug
