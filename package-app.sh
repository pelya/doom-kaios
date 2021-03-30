#!/bin/bash

DIR="$1"
[ -z "$DIR" ] && DIR=debug
cd $DIR || exit 1

LEVEL=
[ "$DIR" = "release" ] && LEVEL=-9

cp -f *.html *.js *.mem *.data *.wasm /var/www/html

rm -f application.zip
zip $LEVEL application.zip *.html *.js *.mem *.data
cd ../app
zip $LEVEL ../$DIR/application.zip *.png *.jpg *.html *.ico *.js *.gif *.webapp
