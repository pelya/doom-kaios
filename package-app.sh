#!/bin/bash

DIR="$1"
[ -z "$DIR" ] && DIR=release
cd $DIR || exit 1

cp -f *.html *.js *.mem *.data /var/www/html

rm -f application.zip
zip -9 application.zip *.html *.js *.mem *.data
cd ../app
zip -9 ../$DIR/application.zip *.png *.jpg *.html *.ico *.js *.gif *.webapp
