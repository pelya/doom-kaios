#!/bin/bash

[ -e make-kaios-install/Makefile ] || git submodule update --init --recursive

echo "Download XULRunner from http://ftp.mozilla.org/pub/mozilla.org/xulrunner/releases/18.0.2/sdk/ "

export LD_LIBRARY_PATH=~/bin/xulrunner-sdk/bin


FOLDER=`pwd`/out
[ -z "$FOLDER" ] && FOLDER=release
ID=doom

adb push ${FOLDER}/application.zip /data/local/tmp/b2g/$ID/application.zip || exit 1

cd build/make-kaios-install

make FOLDER=$FOLDER ID=$ID install || exit 1
