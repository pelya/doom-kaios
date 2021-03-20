#!/bin/sh


convert -delay 20 -loop 0 -dispose previous -gravity center -extent 56x56 -background 'rgba(0,0,0,0)' \
skulc1.png skulc8c2.png skulc7c3.png skulc6c4.png skulc5.png skulc6c4-flip.png skulc7c3-flip.png skulc8c2-flip.png \
icon_56.gif

gif2apng icon_56.gif ../icon_56.png || { echo "sudo apt-get install gif2apng"; exit 1; }

convert -delay 20 -loop 0 -dispose previous -gravity center -extent 56x56 -background 'rgba(0,0,0,0)' -filter point -resize 200% \
skulc1.png skulc8c2.png skulc7c3.png skulc6c4.png skulc5.png skulc6c4-flip.png skulc7c3-flip.png skulc8c2-flip.png \
icon_112.gif

gif2apng icon_112.gif ../icon_112.png
