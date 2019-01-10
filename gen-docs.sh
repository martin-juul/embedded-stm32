#!/usr/bin/env bash

DOXYFILE=$(readlink -f Doxyfile)

rm -rf ./docs/html

doxygen $DOXYFILE

git add .