#!/bin/bash

ALPHA=

convert icon.png $ALPHA -resize 16x16 -depth 32 16-32.png
convert icon.png $ALPHA -resize 32x32 -depth 32 32-32.png
convert icon.png $ALPHA -resize 48x64 -depth 32 48-32.png
convert icon.png $ALPHA -resize 256x256 -depth 32 256-32.png
convert 16-32.png 32-32.png 48-32.png 256-32.png ../xerodashboard/Icon.ico




