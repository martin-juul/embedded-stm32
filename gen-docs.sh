#!/usr/bin/env bash

DIR="$(cd -P "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

DOXYFILE="${DIR}/Doxyfile"

rm -rf ./docs/html

doxygen "${DOXYFILE}"

#git add .