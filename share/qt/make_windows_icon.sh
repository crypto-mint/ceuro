#!/bin/bash
# create multiresolution windows icon
ICON_SRC=../../src/qt/res/icons/eurocoin.png
ICON_DST=../../src/qt/res/icons/eurocoin.ico
convert ${ICON_SRC} -resize 16x16 eurocoin-16.png
convert ${ICON_SRC} -resize 32x32 eurocoin-32.png
convert ${ICON_SRC} -resize 48x48 eurocoin-48.png
convert eurocoin-16.png eurocoin-32.png eurocoin-48.png ${ICON_DST}

