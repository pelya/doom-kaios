#!/bin/bash

[ -z "`which emsdk`" ] && export PATH=`pwd`/../emsdk:$PATH

[ -z "`which emsdk`" ] && { echo "Put emsdk into your PATH"; exit 1 ; }

[ -z "$PATH_EMSDK" ] && PATH_EMSDK="`which emsdk | xargs dirname`"

source "$PATH_EMSDK/emsdk_env.sh"

[ -e Makefile ] || emcmake cmake -DCMAKE_BUILD_TYPE=Debug . || exit 1

emmake make -j8

cp -f *.html *.js *.mem *.data /var/www/html

rm -f application.zip
zip -9 application.zip *.html *.js *.mem *.data
