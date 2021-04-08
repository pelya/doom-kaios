#!/bin/bash

DIR="$1"
[ -z "$DIR" ] && DIR=debug

cd app
cp -f *.webapp *.html *.js *.png *.jpg *.ico *.gif ../$DIR
cd ..

cd $DIR || exit 1

LEVEL=
[ "$DIR" = "release" ] && LEVEL=-9
[ "$DIR" = "debug" ] && sed -i 's/var sys_debug = 0;/var sys_debug = 1;/' sys.js

cp -f *.webapp *.html *.js *.mem *.data *.wasm *.png *.jpg *.ico *.gif /var/www/html/

rm -f application.zip
zip $LEVEL application.zip *.webapp *.html *.js *.mem *.data *.wasm *.png *.jpg *.ico *.gif
