#!/usr/bin/env bash

set -e

DIR="$(cd -P "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DOXYFILE="${DIR}/Doxyfile"

echo -e "Removing existing generated files if any."

find "${DIR}/docs" ! -name '_config.yml' -type f -exec rm -f {} +
rm -rf "${DIR}/docs/search"

echo -e "Generating docs"
doxygen "${DOXYFILE}"

echo -e "Overwriting docs directory"
(cd "${DIR}/docs/html/" && mv ./* ../)

echo -e "Removing artifacts"
rm -rf "${DIR}/docs/html"

echo -e "Adding files to git"
git add .